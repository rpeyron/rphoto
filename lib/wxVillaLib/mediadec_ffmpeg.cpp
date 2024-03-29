/////////////////////////////////////////////////////////////////////////////
// Name:        mediadec_ffmpeg.cpp
// Purpose:     FFMPEG Media Decoder
// Author:      Alex Thuering
// Created:     21.07.2007
// RCS-ID:      $Id: mediadec_ffmpeg.cpp,v 1.1 2007/11/15 20:46:31 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "mediadec_ffmpeg.h"
#include <wx/wx.h>

#define INT64_C(val) val##LL
#define UINT64_C(val) val##ULL
extern "C" {
#define __STDC_CONSTANT_MACROS
#define __STDC_LIMIT_MACROS
#include <ffmpeg/avformat.h>
#include <ffmpeg/swscale.h>
}

wxFfmpegMediaDecoder::wxFfmpegMediaDecoder():
  m_formatCtx(NULL), m_codecCtx(NULL), m_frame(NULL)
{
}

wxFfmpegMediaDecoder::~wxFfmpegMediaDecoder()
{
    Close();
}

void wxFfmpegMediaDecoder::Init()
{
    av_register_all();
}

bool wxFfmpegMediaDecoder::Load(const wxString& fileName)
{
    Close();
    if (av_open_input_file(&m_formatCtx, fileName.mb_str(), NULL, 0, NULL) !=0)
       return false;
    // Retrieve stream information
    if (av_find_stream_info(m_formatCtx)<0)
       return false;
    return true;
}

void wxFfmpegMediaDecoder::Close()
{
    EndDecode();
    if (m_formatCtx)
        av_close_input_file(m_formatCtx);
    m_formatCtx = NULL;
}

double wxFfmpegMediaDecoder::GetDuration()
{
    return m_formatCtx != NULL && m_formatCtx->duration != AV_NOPTS_VALUE ?
      m_formatCtx->duration / AV_TIME_BASE : -1;
}

wxSize wxFfmpegMediaDecoder::GetVideoSize()
{
    OpenVideoDecoder();
    return m_codecCtx ? wxSize(m_codecCtx->width, m_codecCtx->height) : wxSize();
}

bool wxFfmpegMediaDecoder::OpenVideoDecoder()
{
    if (m_codecCtx)
        return true;
    // find the first video stream
    m_videoStream = -1;
    for (int i=0; i<m_formatCtx->nb_streams; i++)
    {
        if (m_formatCtx->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO)
        {
            m_videoStream = i;
            break;
        }
    }
    if(m_videoStream == -1)
        return false;
    // get a pointer to the codec context for the video stream 
    m_codecCtx = m_formatCtx->streams[m_videoStream]->codec;
    // find and open the decoder for the video stream 
    AVCodec* codec = avcodec_find_decoder(m_codecCtx->codec_id);
    if (!codec || avcodec_open(m_codecCtx, codec)<0)
    {
        m_codecCtx = NULL;
        return false;
    }
    return true;
}

void wxFfmpegMediaDecoder::CloseVideoDecoder()
{
    if (m_codecCtx)
        avcodec_close(m_codecCtx); // close the codec
    m_codecCtx = NULL;
}

bool wxFfmpegMediaDecoder::BeginDecode(int width, int height)
{
    if (!m_formatCtx || !OpenVideoDecoder())
        return false;
    // video size
    if (width == -1 || height == -1)
    {
        m_width = m_codecCtx->width;
        m_height = m_codecCtx->height;
    }
    else
    {
        int w = m_codecCtx->width * height / m_codecCtx->height;
        int h = m_codecCtx->height * width / m_codecCtx->width;
        m_width = w < width ? w : width;
        m_height = w < width ? height : h;
    }
    // allocate video frame
    m_frame = avcodec_alloc_frame();
    if (!m_frame) {
        avcodec_close(m_codecCtx);
        m_codecCtx = NULL;
        return false;
    }
    return true;
}

bool wxFfmpegMediaDecoder::SetPosition(double pos)
{
    if (m_formatCtx == NULL)
        return false;
    int64_t timestamp = pos * AV_TIME_BASE;
    if (m_formatCtx->start_time != AV_NOPTS_VALUE)
        timestamp += m_formatCtx->start_time;
    return av_seek_frame(m_formatCtx, -1, timestamp, AVSEEK_FLAG_BACKWARD) >= 0;
}

wxImage wxFfmpegMediaDecoder::GetNextFrame()
{
    if (!m_frame && !BeginDecode())
        return wxImage();
    
    int frameFinished;
    AVPacket packet;
    while (av_read_frame(m_formatCtx, &packet) >=0)
    {
      // is this a packet from the video stream?
      if(packet.stream_index == m_videoStream)
      {
        // decode video frame
        avcodec_decode_video(m_codecCtx, m_frame, &frameFinished, packet.data, packet.size);        
        if (frameFinished)
        {
            SwsContext* imgConvertCtx = sws_getContext(m_codecCtx->width, m_codecCtx->height,
                m_codecCtx->pix_fmt, m_width, m_height, PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
            if (imgConvertCtx == NULL)
                return wxImage();
            
            wxImage img(m_width, m_height);
            uint8_t *rgbSrc[3]= {img.GetData(), NULL, NULL};
            int rgbStride[3]={3*m_width, 0, 0}; 
            sws_scale(imgConvertCtx, m_frame->data, m_frame->linesize,
                0, m_codecCtx->height, rgbSrc, rgbStride);
            av_free_packet(&packet);
            return img;
        }
      }
      // free the packet that was allocated by av_read_frame
      av_free_packet(&packet);
    }
    return wxImage();
}

void wxFfmpegMediaDecoder::EndDecode()
{
    if (m_frame)
        av_free(m_frame);
    m_frame = NULL;
    CloseVideoDecoder();
}
