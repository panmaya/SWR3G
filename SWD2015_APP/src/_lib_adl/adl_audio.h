/*
 * adl_audio.h
 *
 * Created: 17/2/2558 9:50:34
 *  Author: pan
 */ 


#ifndef ADL_AUDIO_H_
#define ADL_AUDIO_H_

/**********************************************************************************************************************/
/*  adl_audio.h   -  Copyright Sierra Wireless Inc. (c) 2010														  */
/*                                                                                                                    */
/*                                                                                                                    */
/* LICENSE                                                                                                            */
/* =======                                                                                                            */
/* If no specific agreement has been signed, this Source Code is subject to the following license terms:              */
/* http://www.sierrawireless.com/productsandservices/AirPrime/Sierra_Wireless_Software_Suite/License/Userlicense.aspx */
/* If you modify this file, you are subject to the contribution rules described into this license                     */
/**********************************************************************************************************************/


/**
  @file adl_audio.h
  @brief ADL Audio Service public interface header file
  @ingroup adl_audio_public_IFS
  */

/**
  @ifnot GLOBAL_IFS
  @mainpage
  @else
  @defgroup adl_audio_public_IFS ADL Audio Service Interface Specification
  @endif

  @section adl_audio_public_IFS_OVERVIEW Overview
  This document describes the public Audio Service interface of the ADL software element.

  @section adl_audio_public_IFS_INTRODUCTION Introduction

  @subsection adl_audio_public_IFS_REMINDER Reminder

  Interfaces are the means by which software elements interact. An interface is a list of operations providing a coherent service.
  Software elements should have as much (independent) interfaces than of managed services.
  Uses the interface specification
  - To software element developers: provides the contract that states what the provider has to implement to meet the services promised by the interface.
  - To software element users: provides the contract that states what the client needs to do to use the interface.

  @subsection adl_audio_public_IFS_RELATED_DOCUMENTS Related documents references

  Here are the documents referenced in this interface specification.
  -# @anchor DocRef2 <a href="Notes://domino-arc/C1256B52003391E9/C1256F9300391ADBC12570AD0039A948/A7372BF2E677A744C125721400336611" target="_blank">FCS : X62 Downlink DTMF Decoding</a>

  @subsection adl_audio_public_IFS_ABBR_AND_GLOSSARY Abbreviations and glossary


  - <b>N.A.</b>\n
    Not appliable
  - <b>N.U.</b>\n
    Not used
  - <b>IFS</b>\n
    Interface Specification
  - <b>SE</b>\n
    Software Element
  - <b>PCM</b>\n
    Pulse Coded Modulation
  - <b>DTMF</b>\n
    Dual Tone Multi-Frequency
  - <b>AMR</b>\n
    Adaptative Multi Rate
  - <b>AMR-WB</b>\n
    AMR-WideBand
  - <b>R</b>\n
    Only readable.
  - <b>RW</b>\n
    Readable & writable.
  */

/**
  @ifnot GLOBAL_IFS
  @defgroup adl_audio_public_IFS ADL Audio Service Interface Specification
  @else
  @addtogroup adl_audio_public_IFS
  @{
  @endif

  @section adl_audio_public_IFS_ftr Features

  The ADL supplies Audio Service interface to applications in order to handle audio resources,
  and play or listen supported audio formats on these resources (single/dual tones, DTMF tones, melodies, PCM audio streams,
  decoded DTMF streams).

  The defined operations are:
  - An <b>adl_audioSubscribe()</b> function to subscribe to an audio resource.
  - An <b>adl_audioUnsubscribe()</b> function to unsubscribe from an audio resource.
  - An <b>adl_audioTonePlay()</b> function to play a single/dual tone.
  - An <b>adl_audioDTMFPlay()</b> function to play a DTMF tone.
  - An <b>adl_audioMelodyPlay()</b> function to play a melody.
  - An <b>adl_audioTonePlayExt()</b> function to play a single/dual tone (extension).
  - An <b>adl_audioDTMFPlayExt()</b> function to play a DTMF tone (extension).
  - An <b>adl_audioMelodyPlayExt()</b> function to play a melody (extension).
  - An <b>adl_audioStreamPlay()</b> function to play an audio stream.
  - An <b>adl_audioStreamListen()</b> function to listen to an audio stream.
  - An <b>adl_audioStop()</b> function to stop playing or listenning.
  - An <b>adl_audioSetOption()</b> function to set audio options.
  - An <b>adl_audioGetOption()</b> function to get audio options.



  @if GLOBAL_IFS
  @}
  @endif
  */



#ifndef __adl_audio_H__
#define __adl_audio_H__





/**
  @defgroup adl_audio_public_IFS_Definitions Types and constants definition
  @ingroup adl_audio_public_IFS

  */


/**
  * @addtogroup adl_audio_public_IFS_Definitions
  * @{
  */





/**
  @brief DTMF per frame

  @par Description:
  This constant defines maximal number of received DTMFs each time interrupt handlers are called when a listening to a DTFM stream in <b>Raw mode</b> ( refer to #ADL_AUDIO_DTMF_DETECT_BLANK_DURATION for more information about Raw mode ).
 */
#define ADL_AUDIO_MAX_DTMF_PER_FRAME   2





/**
  @brief Wireless CPU audio resources.

  This type lists the available audio resources of the Wireless CPU, including the local ones
  (plugged to the Wireless CPU itself) and the ones related to any running voice call.
  These resources are usable either to play a pre-defined/stream audio format (output resources),
  or to listen to an incoming audio stream (input resources).
 */
typedef enum _adl_audioResources_e
{
    ADL_AUDIO_SPEAKER,           ///< Current speaker (output resource; please refer to the AT Command interface guide for more information on how to select the current speaker).
    ADL_AUDIO_BUZZER,            ///< Buzzer (output resource, just usable to play single frequency tones & melodies)
    ADL_AUDIO_MICROPHONE,        ///< Current microphone (input resource; please refer to the AT Command interface guide for more information on how to select the current microphone).
    ADL_AUDIO_VOICE_CALL_RX,     ///< Running voice call incoming channel (input resource, available when a voice call is running to listen to audio streams)
    ADL_AUDIO_VOICE_CALL_TX,     ///< Running voice call outgoing channel (output resource, available when a voice call is running to play audio streams)
} adl_audioResources_e;




/**
  @brief Audio resource subscription options.

  Options set to be used at resource subscription time.
 */
typedef enum _adl_audioResourceOption_e
{
    ADL_AUDIO_RESOURCE_OPTION_FORBID_PREEMPTION = 0x00,     ///< Never allows prioritary uses of the resource (the resource subscriber owns the resource until unsubscription time)
    ADL_AUDIO_RESOURCE_OPTION_ALLOW_PREEMPTION = 0x01,      ///< Allows prioritary uses of the ressource (such as incoming voice call melody, outgoing voice call tone play, SIM Toolkit application tone play).
} adl_audioResourceOption_e;




/**
  @brief Audio stream formats.

  Available formats for audio stream playing/listening processes.
 */
typedef enum _adl_audioFormats_e
{
    ADL_AUDIO_DTMF,                ///< Decoded DTMF sequence.
    ADL_AUDIO_PCM_MONO_8K_16B,     ///< PCM mono 16 bits / 8 KHz Audio sample.
    ADL_AUDIO_PCM_MONO_16K_16B,    ///< PCM mono 16 bits / 16 KHz Audio sample.
    ADL_AUDIO_AMR                  ///< AMR / AMR-WB Audio sample
} adl_audioFormats_e ;


/**
  @brief Audio interrupt source instance.

  Instance set of the audio interrupt event which occurs when audio stream listening or playing is started. Refer to <b>Instance</b> field in #adl_irqEventData_t structure to get more information.
 */
typedef enum _adl_audioInstance_e
{
    ADL_AUDIO_DTMF_INSTANCE,          ///< For DTMF decoding interruption.
    ADL_AUDIO_PCM_INSTANCE,           ///< When audio stream recording or playing is started with #ADL_AUDIO_PCM_MONO_8K_16B or #ADL_AUDIO_PCM_MONO_16K_16B format.
    ADL_AUDIO_AMR_INSTANCE            ///< When audio stream recording or playing is started with #ADL_AUDIO_AMR format.
} adl_audioInstance_e ;


/**
  @brief AMR codec rate available.

  Available speech codec rate for audio ARM / AMR-WB stream playing process.
 */
typedef enum _adl_audioAmrCodecRate_e
{
    ADL_AUDIO_AMR_RATE_4_75,            ///< AMR codec rate 4.75 kb/s
    ADL_AUDIO_AMR_RATE_5_15,            ///< AMR codec rate 5.15 kb/s
    ADL_AUDIO_AMR_RATE_5_90,            ///< AMR codec rate 5.90 kb/s
    ADL_AUDIO_AMR_RATE_6_70,            ///< AMR codec rate 6.70 kb/s
    ADL_AUDIO_AMR_RATE_7_40,            ///< AMR codec rate 7.40 kb/s
    ADL_AUDIO_AMR_RATE_7_95,            ///< AMR codec rate 7.95 kb/s
    ADL_AUDIO_AMR_RATE_10_20,           ///< AMR codec rate 10.20 kb/s
    ADL_AUDIO_AMR_RATE_12_20,           ///< AMR codec rate 12.20 kb/s
    ADL_AUDIO_AMR_WB_RATE_6_60,         ///< AMR-WB codec rate 6.60 kb/s, refer to ADL_#AUDIO_AMR_WB_AVAILABLE.
    ADL_AUDIO_AMR_WB_RATE_8_85,         ///< AMR-WB codec rate 8.85 kb/s, refer to ADL_#AUDIO_AMR_WB_AVAILABLE.
    ADL_AUDIO_AMR_WB_RATE_12_65,        ///< AMR-WB codec rate 12.65 kb/s, refer to ADL_#AUDIO_AMR_WB_AVAILABLE.
    ADL_AUDIO_AMR_WB_RATE_14_25,        ///< AMR-WB codec rate 14.25 kb/s, refer to ADL_#AUDIO_AMR_WB_AVAILABLE.
    ADL_AUDIO_AMR_WB_RATE_15_85,        ///< AMR-WB codec rate 15.85 kb/s, refer to ADL_#AUDIO_AMR_WB_AVAILABLE.
    ADL_AUDIO_AMR_WB_RATE_18_25,        ///< AMR-WB codec rate 18.25 kb/s, refer to ADL_#AUDIO_AMR_WB_AVAILABLE.
    ADL_AUDIO_AMR_WB_RATE_19_85,        ///< AMR-WB codec rate 19.85 kb/s, refer to ADL_#AUDIO_AMR_WB_AVAILABLE.
    ADL_AUDIO_AMR_WB_RATE_23_05,        ///< AMR-WB codec rate 23.05 kb/s, refer to ADL_#AUDIO_AMR_WB_AVAILABLE.
    ADL_AUDIO_AMR_WB_RATE_23_85,        ///< AMR-WB codec rate 23.85 kb/s, refer to ADL_#AUDIO_AMR_WB_AVAILABLE.
} adl_audioAmrCodecRate_e ;





/**
  @brief  Audio events.

  Set of events which will be notified by ADL to audio event handlers.
 */
typedef enum _adl_audioEvents_e
{
    ADL_AUDIO_EVENT_NORMAL_STOP,         ///< A pre-defined audio format play has ended (please refer to adl_audioDTMFPlay(), adl_audioTonePlay() or adl_audioMelodyPlay() for more information)\n
                                         ///< This event is not sent on a request to stop from application.

    ADL_AUDIO_EVENT_RESOURCE_RELEASED    ///< Resource has been automatically unsubscribed due to a prioritary use by the Wireless CPU (please refer to the #ADL_AUDIO_RESOURCE_OPTION_ALLOW_PREEMPTION option and adl_audioSubscribe() for more information)
} adl_audioEvents_e ;


/**
  @brief  Audio service options.

  Set of options readable and writable through the adl_audioSetOption() & adl_audioGetOption() functions.
  These options allow to configure the Wireless CPU audio service behaviour, and to get this audio service capabilities and parameters ranges.\n
  For each option, the value type is specified, and a specific keyword indicates the option access:
  - <b>R:</b> the option is only readable.
  - <b>RW:</b> the option is both readable & writable.\n

  (For more information about indicative values which should be returned when reading options for MIN/MAX values, please refer to the Audio Commands chapter of the AT Commands Interface Guide)
 */
typedef enum _adl_audioOptionTypes_e
{
    ADL_AUDIO_DTMF_DETECT_BLANK_DURATION,    ///< <b>RW:</b> DTMF decoding option (u16); it allow to define the blank duration (ms) in order to detect the end of a DTMF. This value will act on the Wireless CPU behaviour to return information about DTMF when listening to a DTMF audio stream. The value has to be a multiple of value returned by #ADL_AUDIO_RAW_DTMF_SAMPLE_DURATION option multiplied by #ADL_AUDIO_MAX_DTMF_PER_FRAME . \n
                                             ///< If a NULL value is specified, DTMF decoder will be in <b>Raw mode</b> (default), Raw datas coming from DTMF decoder are sent via interrupt handlers with a frequency which depends on value returned by #ADL_AUDIO_RAW_DTMF_SAMPLE_DURATION option multiplied by #ADL_AUDIO_MAX_DTMF_PER_FRAME . This mode implies to implement an algorithm in order to detect the good DTMF. (refer to  #_adl_audioDecodedDtmf_u::DecodedDTMFChars for more information about buffer type used) \n
                                             ///< Otherwise the Raw mode is disabled. The value specifies the blank duration which notifies the end of DTMF. Each time a DTMF is detected , interrupt handlers are called.(refer to  #adl_audioPostProcessedDecoder_t structure for more information about datas stored)

    ADL_AUDIO_MAX_FREQUENCY,                 ///< <b>R:</b> allows to get the maximum frequency allowed to be played on the required output resource (please refer to #adl_audioResourceOption_e for more information). the returned frequency value is defined in Hz (u16).
    ADL_AUDIO_MIN_FREQUENCY,                 ///< <b>R:</b> allows to get the minimum frequency allowed to be played on the required output resource (please refer to #adl_audioResourceOption_e for more information). the returned frequency value is defined in Hz (u16).
    ADL_AUDIO_MAX_GAIN,                      ///< <b>R:</b> supplies the maximum gain which can be set to play a pre-defined audio format (please refer to adl_audioDTMFPlayExt(), adl_audioTonePlayExt() or adl_audioMelodyPlayExt() for more information). the returned gain value is defined in 1/100 of dB (s16). This value can be retrieved only with #ADL_AUDIO_SPEAKER and #ADL_AUDIO_BUZZER  audio resource handle otherwise an error will be returned.
    ADL_AUDIO_MIN_GAIN,                      ///< <b>R:</b> supplies the minimum gain which can be set to play a pre-defined audio format (please refer to adl_audioDTMFPlayExt(), adl_audioTonePlayExt() or adl_audioMelodyPlayExt() for more information). the returned gain value is defined in 1/100 of dB (s16). This value can be retrieved only with #ADL_AUDIO_SPEAKER and #ADL_AUDIO_BUZZER  audio resource handle otherwise an error will be returned.
    ADL_AUDIO_MAX_DURATION,                  ///< <b>R:</b> supplies the maximum duration which can be set to play a DTMF tone or a single/dual tone (please refer to adl_audioDTMFPlay() or adl_audioTonePlay() for more information). the returned duration value is defined in ms (u32). This value can be retrieved only with #ADL_AUDIO_SPEAKER and #ADL_AUDIO_BUZZER  audio resource handle otherwise an error will be returned.
    ADL_AUDIO_MIN_DURATION,                  ///< <b>R:</b> supplies the minimum duration which can be set to play a DTMF tone or a single/dual tone (please refer to adl_audioDTMFPlay() or adl_audioTonePlay() for more information). the returned duration value is defined in ms (u32). This value can be retrieved only with #ADL_AUDIO_SPEAKER and #ADL_AUDIO_BUZZER  audio resource handle otherwise an error will be returned.
    ADL_AUDIO_MAX_NOTE_VALUE,                ///< <b>R:</b> supplies the maximum duration for a note (tempo) which can be set to play a melody (please refer to adl_audioMelodyPlay() for more information). This value is the maximal value which can be defined with #ADL_AUDIO_NOTE_DEF macro (u32).
    ADL_AUDIO_MIN_NOTE_VALUE,                ///< <b>R:</b> supplies the minimum duration for a note (tempo) which can be set to play a melody (please refer to adl_audioMelodyPlay() for more information). This value is the minimal value which can be defined with #ADL_AUDIO_NOTE_DEF macro (u32).
    ADL_AUDIO_DTMF_RAW_STREAM_BUFFER_SIZE,         ///< <b>R:</b> allows to get the buffer type to allocate for listening to a DTMF stream in Raw mode or playing a DTMF stream, defined in number of bytes (u8).
    ADL_AUDIO_DTMF_PROCESSED_STREAM_BUFFER_SIZE,   ///< <b>R:</b> allows to get the buffer type to allocate for listening to a DTMF stream in Pre-processed mode, defined in number of bytes (u8).
    ADL_AUDIO_PCM_8K_16B_MONO_BUFFER_SIZE,    ///< <b>R:</b> allows to get the buffer type to allocated for playing or listening to on a PCM 8KHz 16 bits Mono stream, defined in number of bytes (u16).
    ADL_AUDIO_PCM_16K_16B_MONO_BUFFER_SIZE,   ///< <b>R:</b> allows to get the buffer type to allocated for playing or listening to on a PCM 16KHz 16 bits Mono stream, defined in number of bytes (u16).
    ADL_AUDIO_AMR_WB_AVAILABLE,               ///< <b>R:</b> allows to know if AMR Wideband codec rates are available. TRUE if they're available, FALSE otherwhise (bool). Refer to #adl_audioAmrCodecRate_e to get available codec rates.
    ADL_AUDIO_AMR_SPEECH_CODEC_RATE,          ///< <b>RW:</b> allows to define which codec rate will be used for AMR stream playing. Refer to #adl_audioAmrCodecRate_e to get available codec rates. By default, Codec rate is #ADL_AUDIO_AMR_RATE_4_75.
    ADL_AUDIO_AMR_MIXED_VOICE,                ///< <b>RW:</b> allows to define if the AMR sample should be mixed to the voice when an AMR audio sample is played. This value is set to FALSE to mute vocoder, TRUE otherwise. By default option is set to FALSE (bool).
    ADL_AUDIO_AMR_BUFFER_SIZE,               ///< <b>RW:</b> allows to define the buffer type to allocated for playing or listening to on an AMR stream, defined in number of bytes (u32). By default option is set to 0.\n
                                              ///< According to the selected codec rate, the buffer has to be defined with a multiple of one speech frame size, "0" is not available (refer to adl_audioStreamPlay() to get more information about buffer to allocated.\n
                                              ///< The option value has to match with size of AMR buffer which has been allocated otherwise AMR player(/recorder) risks to not work properly.
    ADL_AUDIO_RAW_DTMF_SAMPLE_DURATION       ///< <b>R:</b> allows to get the duration of one DTMF sample when DTMF decoding is on Raw mode, defined in ms ( u8 ). This value depends on WCPU which is used.

} adl_audioOptionTypes_e ;





/**
  @brief  Post-processed DTMF data structure

  @par Description:
  This structure allows the application to handle post-processed DTMF datas when listening to an audio DTMF stream with Raw mode deactivated.\n
  (refer to #ADL_AUDIO_DTMF_DETECT_BLANK_DURATION for more information about Raw mode)
 */
typedef struct _adl_audioPostProcessedDecoder_t
{
    /**
      @brief Processing metrics

      @par Description:
      contains informations about DTMF decoding process.<b> Reserved for Future Use</b>.
      */
    u32 Metrics;


    /**
      @brief DTMF duration

      @par Description:
      contains post-processed DTMF duration, in ms.
      */
    u32 Duration;


    /**
      @brief PostProcessed DTMF buffer

      @par Description:
      contains decoded DTMF .
      */
    ascii DecodedDTMF;

} adl_audioPostProcessedDecoder_t;



/**
  @brief Audio DTMF buffer union

  @par Description:
  This union defines different types of buffers which are used according to the decoding mode (Raw mode enable or disable) when listening to an audio DTMF stream.\n
  (refer to #ADL_AUDIO_DTMF_DETECT_BLANK_DURATION for more information about Raw mode )
 */
typedef union _adl_audioDecodedDtmf_u
{
    /**
      @brief Raw mode decoded DTMF

      @par Description:
      contains decoded DTMF in Raw mode.
      */
    ascii DecodedDTMFChars [ ADL_AUDIO_MAX_DTMF_PER_FRAME ];


    /**
      @brief Post-processed DTMF

      @par Description:
      contains informations about decoded DTMF and decoding post-process.\n
      ( refer to #adl_audioPostProcessedDecoder_t for more information )
      */
    adl_audioPostProcessedDecoder_t PostProcessedDTMF;

} adl_audioDecodedDtmf_u;





/**
  @brief audio stream data buffer union

  @par Description:
  This union defines differents types of buffers which are used according to the audio format when an audio stream interruption occurs.
 */
typedef union _adl_audioStreamDataBuffer_u
{
    /**
      @brief PCM stream data buffer

      @par Description:
      This buffer is used when playing or listening to an audio PCM stream;
      */
    u8 PCMData [1];

    /**
       @brief AMR stream data buffer

       @par Description:
       This buffer is used when playing to an audio AMR / AMR-WB stream;
       */
    u8 AMRData [1];


    /**
      @brief DTMF stream data buffer

      @par Description:
      This buffer stores decoded DTMF when listening to an audio DTMF stream according to the decoding mode which is used.\n
      (refer to #adl_audioDecodedDtmf_u for more information about DTMF buffer structure and #ADL_AUDIO_DTMF_DETECT_BLANK_DURATION for more information about decoding modes)
      */
    adl_audioDecodedDtmf_u DTMFData;

} adl_audioStreamDataBuffer_u;





/**
  @brief audio stream interruption structure

  @par Description:
  This structure allows the application to handle data buffer according to the audio format when an audio stream interruption occurs during a playing (adl_audioStreamPlay()) or a listening to (adl_audioStreamListen()) an audio stream .
 */
typedef struct _adl_audioStream_t
{
    /**
      @brief Audio format

      @par Description:
      Stream audio format (refer to #adl_audioFormats_e for more information).
      */
    adl_audioFormats_e audioFormat;

    /**
      @brief Data buffer

      @par Description:
      audio data exchange buffer:
      - stores audio sample during an audio PCM stream listening or decoded DTMF during an audio DTMF stream listening.
      - contains audio sample to play during a audio PCM or AMR stream playing.
      (Refer to #adl_audioStreamDataBuffer_u structure for more information)

      */

    adl_audioStreamDataBuffer_u * DataBuffer;


    /**
      @brief Buffer-Ready flag

      @par Description:
      This flag is used for audio stream playing and listening process:
      - When an audio stream is played, each time an interruption occurs this flag has to set to TRUE when data buffer is filled. If this flag is not set to TRUE, an ‘empty’ frame composed of 0x0 will be sent and set the #BufferEmpty flag to TRUE. Once the sample is played #BufferReady is set to FALSE by the firmware.
      - When an audio stream is listened, each time an interruption occurs this flag has to be set to FALSE when data buffer is read. If this flag is not set to FALSE, then firmware will set #BufferOverwrite flag to TRUE.
      <b> This pointer is initialized only when an audio stream is played or listened. Currently, it is used only for PCM stream playing and listening. <\b>
      */
    bool * BufferReady;


    /**
      @brief Buffer-Empty flag

      @par Description:
      When an audio stream is played, this flag is set to TRUE when empty data buffer is played ( for example, when an interruption is missing ). This flag is used just for information, it has to be set to FALSE by application.
      <b> This pointer is initialized only when an audio stream is played. Currently, it is used only for PCM stream playing. <\b>
      */
    bool * BufferEmpty;


    /**
      @brief Buffer-Overwrite flag

      @par Description:
      When an audio stream is listened, this flag is set to TRUE when the last fame has been overwrite( for example, when an interruption is missing ). This flag is used just for information, it has to be set to FALSE by application each time it access the data buffer.
      <b> This pointer is initialized only when an audio stream is listened. Currently, it is used only for PCM stream listening. <\b>
      */
    bool * BufferOverwrite;

} adl_audioStream_t;



/**
  @defgroup DurationNoteDefines Group Notes Durations
  @ingroup adl_audio_public_IFS_Definitions
  @brief Defines notes durations for melody
  */

/**
  * @addtogroup DurationNoteDefines
  * @{
  */

/**
  @brief Whole note

  @par Description:
  The whole note has the longest note duration.
 */
#define  ADL_AUDIO_WHOLE_NOTE    0x10

/**
  @brief Half note

  @par Description:
  The half note has half the duration of a whole note.
 */
#define  ADL_AUDIO_HALF   0x08

/**
  @brief Quarter note

  @par Description:
  The quarter note is the quarter of the whole note.
 */
#define  ADL_AUDIO_QUARTER  0x04

/**
  @brief eighth note

  @par Description:
  The eighth note is a eighth of a whole note.
 */
#define  ADL_AUDIO_EIGHTH   0x02

/**
  @brief Sixteenth note

  @par Description:
  The sixteenth note is a sixteenth of a whole note .
 */
#define  ADL_AUDIO_SIXTEENTH  0x01

/**
  @brief Dotted half note

  @par Description:
  The Dotted half note is equal to three quarter notes .
 */
#define  ADL_AUDIO_DOTTED_HALF  0x0C

/**
  @brief Dotted quarter

  @par Description:
  The dotted quarter note is equal to three eighth notes.
 */
#define  ADL_AUDIO_DOTTED_QUARTER  0x06

/**
  @brief Dotted eighth

  @par Description:
  The dotted eighth note is equal to three sixteenth notes.
 */
#define  ADL_AUDIO_DOTTED_EIGHTH  0x03


/**
  * @}
  */

// deprecated macros
#define ADL_AUDIO_HEIGHTH    ADL_AUDIO_EIGHTH
#define ADL_AUDIO_DOTTED_HEIGHTH    ADL_AUDIO_DOTTED_EIGHTH

/**
  @defgroup IdentificationNoteDefines Group Notes identifications
  @ingroup adl_audio_public_IFS_Definitions
  @brief Defines notes identifications for melody
  */

/**
  * @addtogroup IdentificationNoteDefines
  * @{
  */
/**
  @brief C
 */

#define  ADL_AUDIO_C   0x01
/**
  @brief C #
 */

#define  ADL_AUDIO_CS  0x02
/**
  @brief D
 */

#define  ADL_AUDIO_D   0x03
/**
  @brief D #
 */

#define  ADL_AUDIO_DS  0x04
/**
  @brief E
 */

#define  ADL_AUDIO_E   0x05
/**
  @brief F
 */

#define  ADL_AUDIO_F   0x06
/**
  @brief F #
 */

#define  ADL_AUDIO_FS  0x07

/**
  @brief G
 */
#define  ADL_AUDIO_G   0x08

/**
  @brief G #
 */
#define  ADL_AUDIO_GS  0x09

/**
  @brief A
 */
#define  ADL_AUDIO_A   0x0A

/**
  @brief A #
 */
#define  ADL_AUDIO_AS  0x0B

/**
  @brief B
 */
#define  ADL_AUDIO_B   0x0C

/**
  @brief No sound
 */
#define ADL_AUDIO_NO_SOUND  0xFF

/**
  * @}
  */


/**
  @brief Note Settings

  @par Description:
  This macro is used to define the note value to play according to the note definition, the scale and the note duration.\n
  To play a melody, each note defines in the melody buffer has to be defined with this macro (see adl_audioMelodyPlay() function).

  @param[in]        ID         This parameters corresponds to the note identification. Refer to the \link IdentificationNoteDefines Group Notes identifications \endlink section to see the set of available notes.
  @param[in]        Scale      This parameters defines the note scale (0 - 7).
  @param[in]        Duration   This parameters defines the note duration. Refer to the \link DurationNoteDefines Group Notes Durations \endlink section to see the set of note durations which are available.

  @note to play no sound :
    - ID = #ADL_AUDIO_NO_SOUND
    - Scale = 0

 */
#define ADL_AUDIO_NOTE_DEF( ID, Scale, Duration)    (  ( ( ( ID )  + ( Scale * 12 ) ) << 8 ) + ( Duration ) )



/**
  * @}
  */





/**
  @defgroup adl_audio_public_IFS_cb1 Audio events handler
  @ingroup adl_audio_public_IFS

  */

/**
  * @addtogroup adl_audio_public_IFS_cb1
  * @{
  */

/**
  @par Description:
   such a call-back function has to be supplied to ADL through the adl_audioSubscribe() interface in order to receive audio resource related events.

  @param[in]        audioHandle      This is the handle of the audio resource which is associated to the event (refer to adl_audioSubscribe() for more information about the audio resource handle).
  @param[in]        Event            This is the received event identifier (refer to #adl_audioEvents_e for more information about the different events).

 */
typedef void (*adl_audioEventHandler_f) (  s32 audioHandle, adl_audioEvents_e Event );

/**
  * @}
  */



/**
  @defgroup adl_audio_public_IFS_op1 Audio resources control
  @ingroup adl_audio_public_IFS

  These function allows to allocate or release the set of available resource
  */

/**
  * @addtogroup adl_audio_public_IFS_op1
  * @{
  */




/**
  @par Description:
  This function allows to subscribe to the one of the available resources and specify its behaviour when an other client attempt to subscribe it.\n
  a call-back function is associated for audio resources related events.



  @param[in]        audioResource         Requested audio resource.
  @param[in]        audioEventHandler     application provided audio event call-back function (refer to adl_audioEventHandler_f() for more information.
  @param[in]        Options               option about the audio resource behaviour (refer to #adl_audioResourceOption_e for more information).

  @return   Returned value description
  - <b>Positive or NULL </b>                       if allocation succeed, to be used on further audio API functions calls.
  - <b>#ADL_RET_ERR_PARAM</b>                       if the parameter has an incorrect value.
  - <b>#ADL_RET_ERR_ALREADY_SUBSCRIBED</b>          if the ressource is already subscribed.
  - <b>#ADL_RET_ERR_NOT_SUPPORTED</b>               if the ressource is not supported.
  - <b>#ADL_RET_ERR_SERVICE_LOCKED</b>              if called from a low level interruption handler.

  @note     ERROR values are defined in adl_error.h.
 */
s32 adl_audioSubscribe ( adl_audioResources_e audioResource, adl_audioEventHandler_f audioEventHandler, adl_audioResourceOption_e Options );





/**
  @par Description:
  This function allows to unsubscribe to one of the resources which have been previously subscribed.\n
  A resource can not be unsubscribed if it is running, process on this resource has to be previously stopped (refer to adl_audioStop() for more information).


  @param[in]        audioHandle      Handle of the audio resource which has to be unsubscribed.

  @return   Returned value description
  - <b>OK</b>    on success
  - <b>#ADL_RET_ERR_UNKNOWN_HDL </b>          if the provided handle is unknown.
  - <b>#ADL_RET_ERR_NOT_SUBSCRIBED </b>       if no audio resource has been subscribed.
  - <b>#ADL_RET_ERR_BAD_STATE</b>             if an audio stream is listenning or audio pre-defined signal is playing on the required audio resource.
  - <b>#ADL_RET_ERR_SERVICE_LOCKED</b>        if called from a low level interruption handler.

 */
//s32 adl_audioUnsubscribe ( s32 audioHandle );

/**
  * @}
  */





/**
  @defgroup adl_audio_public_IFS_op2 Play a pre-defined audio format
  @ingroup adl_audio_public_IFS

  These functions allows to play a melody, a tone or a DTMF on the available audio outputs.


  <b> Typical use diagram </b>

  @image html adl_audioPlayPreDefined.png "The following diagram illustrates a typical use of the ADL Audio Service interface to play a predefined audio format."


  */

/**
  * @addtogroup adl_audio_public_IFS_op2
  * @{
  */


/**
  @par Description:
  This function plays a single or dual tone on current speaker and only a single tone on buzzer.\n
  Only the speaker output is able to play tones in two frequencies. The second tone parameters are ignored on buzzer output.\n
  The specified output stops to play at the end of tone duration or on an application request (refer to adl_audioStop() for more information).\n
  Use adl_audioGetOption() function to obtain the parameters range. (also refer to AT command user guide)



  @param[in]        audioHandle                Handle of the audio resource which will play tone (current speaker or buzzer).
  @param[in]        Frequency1                 Frequency for the 1st tone (Hz).
  @param[in]        Gain1                      This parameter sets the tone gain which will be applied to the 1st frequency value (dB).
  @param[in]        Frequency2                 Frequency for the 2nd tone (Hz),only processed on current speaker.\n
                                               Frequency2 has to set to 0 to play a single tone on current speaker.
  @param[in]        Gain2                      This parameter sets the tone gain which will be applied to the 2nd frequency value (dB).
  @param[in]        Duration                   This parameter sets the tone duration (ms). The value has to be a 20-ms multiple.


  @return   Returned value description
  - <b>OK</b>                                on success.
  - <b>#ADL_RET_ERR_PARAM  </b>               if parameters have an incorrect value.
  - <b>#ADL_RET_ERR_UNKNOWN_HDL </b>          if the provided handle is unknown.
  - <b>#ADL_RET_ERR_BAD_STATE</b>             if an audio stream is listenning or audio pre-defined signal is playing on the required audio resource.
  - <b>#ADL_RET_ERR_BAD_HDL</b>               if the audio resource is not allowed for tone playing.
  - <b>#ADL_RET_ERR_NOT_SUPPORTED</b>         if the audio resource is not available for tone playing.
  - <b>#ADL_RET_ERR_SERVICE_LOCKED</b>        if called from a low level interruption handler.

  @note \n
  - an event #ADL_AUDIO_EVENT_NORMAL_STOP is sent to the owner resource when a tone is stopped automatically at the end of the duration time.

  @code
  // audio resource handle
  s32 handle;

  // audio event call-back function
  void MyAudioEventHandler ( s32 audioHandle, adl_audioEvents_e Event )
  {

      switch ( Event)
      {
          case ADL_AUDIO_EVENT_NORMAL_STOP :
              TRACE (( 1, " Audio handle %d : stop ", audioHandle ));

              // unsubscribe to the speaker
              Ret = adl_audioUnsubscribe ( handle );
          break;

          case ADL_AUDIO_EVENT_RESOURCE_RELEASED :
              // ...
          break;

          default : break;
      }
      // ...

      return;
  }

  void adl_main ( adl_InitType_e InitType )
  {
      s32 Ret;

      // Subscribe to the current speaker
      handle = adl_audioSubscribe ( ADL_AUDIO_SPEAKER, MyAudioEventHandler , ADL_AUDIO_RESOURCE_OPTION_FORBID_PREEMPTION );

      // Play a single tone
      Ret = adl_audioTonePlay( handle, 300, -10, 0, 0, 50 );
  }
  @endcode

 */
s32 adl_audioTonePlay (s32 audioHandle, u16 Frequency1, s8 Gain1, u16 Frequency2, s8 Gain2, u32 Duration);





/**
  @par Description:
  This function allows a DTMF tone to be played on the current speaker or on voice call TX (in communication only).\n
  It is not possible to play DTMF on the buzzer.\n
  The specified output stops to play at the end of tone duration or on an application request (refer to adl_audioStop() for more information).\n
  Use adl_audioGetOption() function to obtain the parameters range.(also refer to AT command user guide)


  @param[in]        audioHandle    Handle of the audio resource which will play DTMF tone (current speaker or voice call TX).
  @param[in]        DTMF           DTMF to play (0-9,A-D,*,#).
  @param[in]        Gain           only for the speaker. This parameter sets the tone gain (dB).
  @param[in]        Duration       This parameter sets the tone duration (ms). The value has to be a 20-ms multiple. For voice call TX, duration is not guaranteed, it depends to operator.

  @return   Returned value description
  - <b>OK</b>    on success
  - <b>#ADL_RET_ERR_PARAM  </b>             if parameters have an incorrect value.
  - <b>#ADL_RET_ERR_UNKNOWN_HDL </b>        if the provided handle is unknown.
  - <b>#ADL_RET_ERR_BAD_STATE</b>           if an audio stream is listenning or audio pre-defined signal is playing on the required audio resource.
  - <b>#ADL_RET_ERR_BAD_HDL</b>             if the audio resource is not allowed for DTMF playing.
  - <b>#ADL_RET_ERR_NOT_SUPPORTED</b>       if the audio resource is not available for DTMF playing.
  - <b>#ADL_RET_ERR_SERVICE_LOCKED</b>      if called from a low level interruption handler.

  @note \n
  - an event #ADL_AUDIO_EVENT_NORMAL_STOP is sent to the owner resource when a DTMF is stopped automatically at the end of the duration time.
  - a DTMF can not be stopped on client resquest when DTMF is played on voice call TX.
  - when DTMF is played on voice call TX, no "ADL_AUDIO_EVENT_NORMAL_STOP" is received in audio event handler.


  @code
  // audio resource handle
  s32 handle;

  // audio event call-back function
  void MyAudioEventHandler ( s32 audioHandle, adl_audioEvents_e Event )
  {

      switch ( Event)
      {
          case ADL_AUDIO_EVENT_NORMAL_STOP :
              TRACE (( 1, " Audio handle %d : stop ", audioHandle ));

              // unsubscribe to the current speaker
              Ret = adl_audioUnsubscribe ( handle );
          break;

          case ADL_AUDIO_EVENT_RESOURCE_RELEASED :
              // ...
          break;

          default : break;
      }
      // ...

      return;
  }

  void adl_main ( adl_InitType_e InitType )
  {
      s32 Ret;

      // Subscribe to the current speaker
      handle = adl_audioSubscribe ( ADL_AUDIO_SPEAKER, MyAudioEventHandler , ADL_AUDIO_RESOURCE_OPTION_FORBID_PREEMPTION );

      // Play a DTMF tone
      Ret = adl_audioDTMFPlay( handle, 'A', -10, 10);
  }
  @endcode

 */
//s32 adl_audioDTMFPlay (s32 audioHandle, ascii DTMF, s8 Gain, u32 Duration);





/**
  @par Description:
  This function allows to play a defined melody on current speaker or buzzer.\n
  The specified output stops the playing process on an application request (refer to adl_audioStop() for more information) or
  when the melody has been played the same number of time than that is specified in CycleNumber.\n
  Use adl_audioGetOption() function to obtain the parameters range.(also refer to AT command user guide)

  @param[in]        audioHandle     Handle of the audio resource which will play Melody (current speaker or buzzer).
  @param[in]        MelodySeq       Melody to play. A melody is defined by an u16 table , where each element defines a note event, duration and sound definition.\n
                                    The melody sequence has to finish by a NULL value .\n
                                    (refer to #ADL_AUDIO_NOTE_DEF for more information)
  @param[in]        Tempo           Tempo is defined in bpm (1 beat = 1 quarter note).
  @param[in]        CycleNumber     Number of times the melody should be played. \n
                                    If not specified, the cycle number is infinite, Medoly should be stopped by client.
  @param[in]        Gain            This parameter sets melody gain (dB).

  @return   Returned value description
  - <b>OK</b>    on success
  - <b>#ADL_RET_ERR_PARAM  </b>            if parameters have an incorrect value.
  - <b>#ADL_RET_ERR_UNKNOWN_HDL </b>       if the provided handle is unknown.
  - <b>#ADL_RET_ERR_BAD_STATE</b>          if an audio stream is listenning or audio pre-defined signal is playing on the required audio resource.
  - <b>#ADL_RET_ERR_BAD_HDL</b>            if the audio resource is not allowed for melody playing.
  - <b>#ADL_RET_ERR_NOT_SUPPORTED</b>      if the audio resource is not available for melody playing.
  - <b>#ADL_RET_ERR_SERVICE_LOCKED</b>     if called from a low level interruption handler.

  @note \n
  - an event #ADL_AUDIO_EVENT_NORMAL_STOP is sent to the owner resource when a Melody is stopped automatically at the end of the cycle number.

  @code
  // audio resource handle
  s32 handle;

  // Melody buffer
  u16 * MyMelody = { ADL_AUDIO_NOTE_DEF( ADL_AUDIO_A, 3, ADL_AUDIO_DOTTED_QUARTER),
                     ADL_AUDIO_NOTE_DEF( ADL_AUDIO_CS, 5, ADL_AUDIO_DOTTED_HALF),
                     ADL_AUDIO_NOTE_DEF( ADL_AUDIO_E, 1, ADL_AUDIO_WHOLE_NOTE ),
                     ... ,
                     ADL_AUDIO_NOTE_DEF( ADL_AUDIO_AS, 3, ADL_AUDIO_HEIGHTH),
                     0 };

  // audio event call-back function
  void MyAudioEventHandler ( s32 audioHandle, adl_audioEvents_e Event )
  {
      s32 Ret;

      switch ( Event)
      {
          case ADL_AUDIO_EVENT_NORMAL_STOP :
              TRACE (( 1, " Audio handle %d : stop ", audioHandle ));

              // unsubscribe to the buzzer
              Ret = adl_audioUnsubscribe ( handle );

          break;

          case ADL_AUDIO_EVENT_RESOURCE_RELEASED :
              // ...
          break;

          default : break;
      }
      // ...

      return;
  }

  void adl_main ( adl_InitType_e InitType )
  {
      s32 Ret;

      // Subscribe to the current speaker
      handle = adl_audioSubscribe ( ADL_AUDIO_BUZZER, MyAudioEventHandler , ADL_AUDIO_RESOURCE_OPTION_FORBID_PREEMPTION );

      // Play a Melody
      Ret = adl_audioMelodyPlay( handle, MyMelody, 10, 2, -10);
  }
  @endcode

 */
//s32 adl_audioMelodyPlay ( s32 audioHandle, u16 * MelodySeq, u8 Tempo, u8 CycleNumber, s8 Gain);


/**
  @par Description:
  This function plays a single or dual tone on current speaker and only a single tone on buzzer.\n
  Only the speaker output is able to play tones in two frequencies. The second tone parameters are ignored on buzzer output.\n
  The specified output stops to play at the end of tone duration or on an application request (refer to adl_audioStop() for more information).\n
  Use adl_audioGetOption() function to obtain the parameters range. (also refer to AT command user guide)



  @param[in]        audioHandle                Handle of the audio resource which will play tone (current speaker or buzzer).
  @param[in]        Frequency1                 Frequency for the 1st tone (Hz).
  @param[in]        Gain1                      This parameter sets the tone gain which will be applied to the 1st frequency value (unit : 1/100 of dB).
  @param[in]        Frequency2                 Frequency for the 2nd tone (Hz),only processed on current speaker.\n
                                               Frequency2 has to set to 0 to play a single tone on current speaker.
  @param[in]        Gain2                      This parameter sets the tone gain which will be applied to the 2nd frequency value (unit : 1/100 of dB).
  @param[in]        Duration                   This parameter sets the tone duration (ms). The value has to be a 20ms-multiple.


  @return   Returned value description
  - <b>OK</b>                                on success.
  - <b>#ADL_RET_ERR_PARAM  </b>               if parameters have an incorrect value.
  - <b>#ADL_RET_ERR_UNKNOWN_HDL </b>          if the provided handle is unknown.
  - <b>#ADL_RET_ERR_BAD_STATE</b>             if an audio stream is listenning or audio pre-defined signal is playing on the required audio resource.
  - <b>#ADL_RET_ERR_BAD_HDL</b>               if the audio resource is not allowed for tone playing.
  - <b>#ADL_RET_ERR_NOT_SUPPORTED</b>         if the audio resource is not available for tone playing.
  - <b>#ADL_RET_ERR_SERVICE_LOCKED</b>        if called from a low level interruption handler.

  @note \n
  - an event #ADL_AUDIO_EVENT_NORMAL_STOP is sent to the owner resource when a tone is stopped automatically at the end of the duration time.
*/
//s32 adl_audioTonePlayExt (s32 audioHandle, u16 Frequency1, s16 Gain1, u16 Frequency2, s16 Gain2, u32 Duration);



/**
  @par Description:
  This function allows a DTMF tone to be played on the current speaker or on voice call TX (in communication only).\n
  It is not possible to play DTMF on the buzzer.\n
  The specified output stops to play at the end of tone duration or on an application request (refer to adl_audioStop() for more information).\n
  Use adl_audioGetOption() function to obtain the parameters range.(also refer to AT command user guide)


  @param[in]        audioHandle    Handle of the audio resource which will play DTMF tone (current speaker or voice call TX).
  @param[in]        DTMF           DTMF to play (0-9,A-D,*,#).
  @param[in]        Gain           only for the speaker. This parameter sets the tone gain (unit : 1/100 of dB).
  @param[in]        Duration       This parameter sets the tone duration (ms). The value has to be a 20ms-multiple. For voice call TX, duration is not guaranteed, it depends to operator.

  @return   Returned value description
  - <b>OK</b>    on success
  - <b>#ADL_RET_ERR_PARAM  </b>             if parameters have an incorrect value.
  - <b>#ADL_RET_ERR_UNKNOWN_HDL </b>        if the provided handle is unknown.
  - <b>#ADL_RET_ERR_BAD_STATE</b>           if an audio stream is listenning or audio pre-defined signal is playing on the required audio resource.
  - <b>#ADL_RET_ERR_BAD_HDL</b>             if the audio resource is not allowed for DTMF playing.
  - <b>#ADL_RET_ERR_NOT_SUPPORTED</b>       if the audio resource is not available for DTMF playing.
  - <b>#ADL_RET_ERR_SERVICE_LOCKED</b>      if called from a low level interruption handler.

  @note \n
  - an event #ADL_AUDIO_EVENT_NORMAL_STOP is sent to the owner resource when a DTMF is stopped automatically at the end of the duration time.
  - a DTMF can not be stopped on client resquest when DTMF is played on voice call TX.
  - when DTMF is played on voice call TX, no "ADL_AUDIO_EVENT_NORMAL_STOP" is received in audio event handler.
 */
//s32 adl_audioDTMFPlayExt (s32 audioHandle, ascii DTMF, s16 Gain, u32 Duration);



/**
  @par Description:
  This function allows to play a defined melody on current speaker or buzzer.\n
  The specified output stops the playing process on an application request (refer to adl_audioStop() for more information) or
  when the melody has been played the same number of time than that is specified in CycleNumber.\n
  Use adl_audioGetOption() function to obtain the parameters range.(also refer to AT command user guide)

  @param[in]        audioHandle     Handle of the audio resource which will play Melody (current speaker or buzzer).
  @param[in]        MelodySeq       Melody to play. A melody is defined by an u16 table , where each element defines a note event, duration and sound definition.\n
                                    The melody sequence has to finish by a NULL value .\n
                                    (refer to #ADL_AUDIO_NOTE_DEF for more information)
  @param[in]        Tempo           Tempo is defined in bpm (1 beat = 1 quarter note).
  @param[in]        CycleNumber     Number of times the melody should be played. \n
                                    If not specified, the cycle number is infinite, Medoly should be stopped by client.
  @param[in]        Gain            This parameter sets melody gain (unit : 1/100 of dB).

  @return   Returned value description
  - <b>OK</b>    on success
  - <b>#ADL_RET_ERR_PARAM  </b>            if parameters have an incorrect value.
  - <b>#ADL_RET_ERR_UNKNOWN_HDL </b>       if the provided handle is unknown.
  - <b>#ADL_RET_ERR_BAD_STATE</b>          if an audio stream is listenning or audio pre-defined signal is playing on the required audio resource.
  - <b>#ADL_RET_ERR_BAD_HDL</b>            if the audio resource is not allowed for melody playing.
  - <b>#ADL_RET_ERR_NOT_SUPPORTED</b>      if the audio resource is not available for melody playing.
  - <b>#ADL_RET_ERR_SERVICE_LOCKED</b>     if called from a low level interruption handler.

  @note \n
  - an event #ADL_AUDIO_EVENT_NORMAL_STOP is sent to the owner resource when a Melody is stopped automatically at the end of the cycle number.

 */
//s32 adl_audioMelodyPlayExt ( s32 audioHandle, u16 * MelodySeq, u8 Tempo, u8 CycleNumber, s16 Gain);


/**
  * @}
  */





/**
  @defgroup adl_audio_public_IFS_op3 Audio stream
  @ingroup adl_audio_public_IFS

  These functions allows to play or listen an audio stream

  <b> Typical use diagram </b>

  @image html adl_audioPlayStream.png "The following diagram illustrates a typical use of the ADL Audio Service interface to play a predefined audio format."
  */


/**
  * @addtogroup adl_audio_public_IFS_op3
  * @{
  */


/**
  @par Description:
  This function allows to play an audio sample stream on the current speaker or on voice call TX.\n
  Playing an audio sample stream implies that low level interruption and/or high level interruption have been previously subscribed \n
  (refer to adl_irqSubscribe() in ADL user guide for more information).\n
  Moreover, memory space has to be allocated for the audio stream buffer before playing starts and it has to be released after playing stops.\n
  Each time the interruption related to playing process occurs, allocated buffer has to be filled with audio data to play in IRQ low or high level notification handler.
  Currently, this functions allows to play 3  audio formats :
  - audio signal sampled at 8KHz on 16 bits ( #ADL_AUDIO_PCM_MONO_8K_16B ).
  - audio signal sampled at 16KHz on 16 bits ( #ADL_AUDIO_PCM_MONO_16K_16B ).<b>Only available on current speaker</b>.
  - audio signal compressed by an AMR / AMR-WB codec ( #ADL_AUDIO_AMR ). Refer to #adl_audioAmrCodecRate_e to get more information about available codecs.<b>Playing audio file compressed by AMR-WB codec is only available on current speaker</b>

  @par Play PCM audio format
  Before to start a PCM audio playing process, application has to set audio configuration of WCPU according to sample rate ( 8KHz or 16KHz) of audio file to play. Refer to the
  <b>AT+SPEAKER</b> command in "AT commands interface guide" to get more information about audio resource configuration.\n
  According to audio configuration a different space memory size will be allocated ( use adl_audioGetOption() function to get the size ):
  - refer to #ADL_AUDIO_PCM_8K_16B_MONO_BUFFER_SIZE for a sample rate at 8KHz.
  - refer to #ADL_AUDIO_PCM_16K_16B_MONO_BUFFER_SIZE for a sample rate at 16KHz.\n
  WARNING : If allocated buffer size doesn't match with sample rate, audio playing process risks to not work properly.\n

  @par Play AMR audio format
  This function can play only AMR / AMR-WB audio file stored in RTP format (refer to RFC4867 to get more information about RTP format for AMR and AMR-WB ).
  Before to start an AMR audio playing process, application has to set parameters such as codec rate ( refer to #adl_audioAmrCodecRate_e to get available codec rate ), buffer size ( refer to #ADL_AUDIO_AMR_BUFFER_SIZE to get more information ),  mixed voice option ( refer to #ADL_AUDIO_AMR_MIXED_VOICE to get more information about it ).
  According to audio configuration, an audio signal compressed either with AMR codec or with AMR-WB codec could be played:
  - for an audio signal with sample rate at 8 KHZ, an AMR codec has to be used.
  - for an audio signal with sample rate at 16 KHZ, an AMR-WB codec has to be used. <b>AMR-WB audio recording is only available on speaker</b>
  Refer to the  <b>AT+SPEAKER</b> command in "AT commands interface guide" to get more information about audio resource configuration.\n
  Buffer size, which has to be allocated, depends on the codec rate selected by application. For each codec rate, a minimal space memory size has to be allocated.
  Buffer size has to be either audio AMR file size or multiple of one 20-ms audio AMR speech frame size (this last one depends on codec rate ).\n
  <b>WARNING : 
  - If allocated buffer size doesn't match with codec rate, quality of played audio signal risks to be degraded .
  - when AMR audio file is played on voice call and high level IRQ notification has been subscribed with ADL_IRQ_OPTION_AUTO_READ option (refer to adl_irq.h to get more information about this option) and audio buffer is too huge then a network de-registration may occur. In this case ADL_IRQ_OPTION_AUTO_READ option should not be used or audio buffer size should be a small AMR speech frame size
  </b>
  \n

  @param[in]        audioHandle           Handle of the audio resource which will play audio stream (current speaker or voice call TX).
  @param[in]        audioFormat           Stream audio format. Only #ADL_AUDIO_DTMF format is not available to be played (refer to #adl_audioFormats_e for more information).
  @param[in]        LowLevelIRQHandle     Low level IRQ handle previously returned by IRQ subscription (refer to adl_irqSubscribe() in ADL user guide for more information).
  @param[in]        HighLevelIRQHandle    High level IRQ handle previously returned by IRQ subscription (refer to adl_irqSubscribe() in ADL user guide for more information).
  @param[in]        buffer                contains audio sample to play.

  @return   Returned value description
  - <b>OK</b>    on success
  - <b>#ADL_RET_ERR_PARAM  </b>            if parameters have an incorrect value.
  - <b>#ADL_RET_ERR_UNKNOWN_HDL </b>       if the provided handle is unknown.
  - <b>#ADL_RET_ERR_BAD_STATE</b>          if an audio stream is listenning or audio pre-defined signal is playing on the required audio resource.
  - <b>#ADL_RET_ERR_BAD_HDL</b>            if the audio resource is not allowed for audio stream playing or if interruption handler identifiers are invalid.
  - <b>#ADL_RET_ERR_NOT_SUPPORTED</b>      if the audio resource is not available for audio stream playing.
  - <b>#ADL_RET_ERR_SERVICE_LOCKED</b>     if called from a low level interruption handler.

  @note \n

  - to work properly, LowLevelIRQHandle is mandatory. The low level interruption has to be previously subscribed with #ADL_IRQ_OPTION_AUTO_READ option.
  - the HighLevelIRQHandle is optionnal.
  - each time an audio sample is required , an interrupt handler will be notified to send the data. The interrupt identifier will be set to #ADL_IRQ_ID_AUDIO_RX_PLAY or #ADL_IRQ_ID_AUDIO_TX_PLAY, according to the resource used to start the stream play.
  - in order to works properly, data should be sent in low level interruption handler.
  - some audio filters will be deactivated for audio sample playing (refer to "audio command" chapter in the "AT command Interface Guide" for more information).
  - for audio interruption subscription, #ADL_IRQ_OPTION_POST_ACKNOWLEDGEMENT option is not available.
  - before to play an audio file, header of file has to be removed, only data has to be send.

  @code Start PCM audio playing process

  // audio resource handle
  s32 handle;

  // audio stream buffer
  void * StreamBuffer;

  // PCM samples
  u16 PCM_Samples[160] = { ... , ... , ... , ... , ... , ... , 0 };   // size of PCM sample = 320 bytes

  // PCM samples index
  u8 indexPCM = 0;

  // Low level interruption handler
  bool MyLowLevelIRQHandler ( adl_irqID_e Source, adl_irqNotificationLevel_e Notification Level, adl_irqEventData_t * Data )
  {
      // copy PCM sample to play
      wm_strcpy( StreamBuffer, PCM_Samples );
      // Set BufferReady flag to TRUE
      *( ( adl_audioStream_t * )Data->SourceData )->BufferReady = TRUE;

      //...

      return FALSE;
  }

  // audio event call-back function
  void MyAudioEventHandler ( s32 audioHandle, adl_audioEvents_e Event )
  {

      // ...

      return;
  }

  void adl_main ( adl_InitType_e InitType )
  {
      s32 Ret;
      s32 BufferSize;

      // Subscribe to the current speaker
      handle = adl_audioSubscribe ( ADL_AUDIO_SPEAKER, MyAudioEventHandler , ADL_AUDIO_RESOURCE_OPTION_FORBID_PREEMPTION );

      // Memory allocation
      Ret = adl_audioGetOption ( handle, ADL_AUDIO_PCM_8K_16B_MONO_BUFFER_SIZE, &BufferSize )
      StreamBuffer = adl_memGet( BufferSize );   // release memory after audio stream playing

      // Play an audio PCM stream
      Ret = adl_audioStreamPlay( handle, ADL_AUDIO_PCM_MONO_8K_16B MyLowLevelIRQHandler, 0, StreamBuffer);
  }
  @endcode

  @code Start AMR audio playing process

  #define AMR_SIZE = 160

  // audio resource handle
  s32 handle;

  // audio stream buffer
  void * StreamBuffer;

  // AMR samples
  u8 AMR_Samples[AMR_SIZE] = { ... , ... , ... , ... , ... , ... , 0 };   // size of AMR audio sample to play = 160 bytes

  // Low level interruption handler
  bool MyLowLevelIRQHandler ( adl_irqID_e Source, adl_irqNotificationLevel_e Notification Level, adl_irqEventData_t * Data )
  {
      // copy PCM sample to play
      wm_strcpy( StreamBuffer, AMR_Samples );

      //...

      return FALSE;
  }

  // audio event call-back function
  void MyAudioEventHandler ( s32 audioHandle, adl_audioEvents_e Event )
  {

      // ...

      return;
  }

  void adl_main ( adl_InitType_e InitType )
  {
      s32 Ret;
      s32 BufferSize = AMR_SIZE;
      bool MixedOption = FALSE;
      adl_audioAmrCodecRate_e CodecRate = ADL_AUDIO_AMR_RATE_5_15;

      // Subscribe to the current speaker
      handle = adl_audioSubscribe ( ADL_AUDIO_SPEAKER, MyAudioEventHandler , ADL_AUDIO_RESOURCE_OPTION_FORBID_PREEMPTION );

      // Set Mixed voice option
      Ret = adl_audioSetOption ( handle, ADL_AUDIO_AMR_MIXED_VOICE, &MixedOption );

      // Set Codec Rate
      Ret = adl_audioSetOption ( handle, ADL_AUDIO_AMR_CODEC_RATE, &CodecRate )

      // Memory allocation
      Ret = adl_audioSetOption ( handle, ADL_AUDIO_AMR_BUFFER_SIZE, &BufferSize );
      StreamBuffer = adl_memGet( BufferSize );   // release memory after audio stream playing

      // Play an audio AMR stream
      Ret = adl_audioStreamPlay( handle, ADL_AUDIO_AMR, MyLowLevelIRQHandler, 0, StreamBuffer);
  }
  @endcode

 */
//s32 adl_audioStreamPlay (s32 audioHandle, adl_audioFormats_e audioFormat,  s32 LowLevelIRQHandle, s32 HighLevelIRQHandle, void * buffer );





/**
  @par Description:
  This function allows listening to a DTMF tone or an audio sample from microphone or voice call RX.\n
  Listening to an audio sample stream implies that low level interruption and/or high level interruption have been previously subscribed \n
  (refer to adl_irqSubscribe() in ADL user guide for more information).\n
  Moreover, memory space has to be allocated for the audio stream buffer before listening starts and it has to be released after listening stops.\n
  Each time the interruption related to playing process occurs, recorded audio data has to be saved in allocated buffer in IRQ low or high level notification handler.
  Currently, this functions allows to record 4 audio formats :
  - decoded DTMF ( #ADL_AUDIO_DTMF ).
  - audio signal sampled at 8KHz on 16 bits ( #ADL_AUDIO_PCM_MONO_8K_16B ).
  - audio signal sampled at 16KHz on 16 bits ( #ADL_AUDIO_PCM_MONO_16K_16B ). <b>Only available on microphone</b>
  - audio signal compressed by an AMR or AMR-WB codec ( #ADL_AUDIO_AMR ). Refer to #adl_audioAmrCodecRate_e to get more information about available codecs.<b>Recording with AMR-WB codec is only available on microphone</b>

  @par DTMF decoding
  Function allow to listen to a DTMF stream in Raw mode or in Pre-processed mode according to blank duration set initially. (refer to #ADL_AUDIO_DTMF_DETECT_BLANK_DURATION for more information about Raw mode)\n
  According to the mode of DTMF decoding, a different buffer size has to be allocated :
  - for Raw mode , refer to #ADL_AUDIO_DTMF_RAW_STREAM_BUFFER_SIZE
  - for Pre-processed mode , refer to #ADL_AUDIO_DTMF_PROCESSED_STREAM_BUFFER_SIZE

  @par Record PCM audio format
  Before to start a PCM audio recording process, application has to set audio configuration of WCPU to define recording sample rate . Refer to the
  <b>AT+SPEAKER</b> command in "AT commands interface guide" to get more information about audio resource configuration.\n
  According to audio configuration a different space memory size will be allocated ( use adl_audioGetOption() function to get the size ):
  - refer to #ADL_AUDIO_PCM_8K_16B_MONO_BUFFER_SIZE for a sample rate at 8KHz.
  - refer to #ADL_AUDIO_PCM_16K_16B_MONO_BUFFER_SIZE for a sample rate at 16KHz.\n
  
  WARNING : If allocated buffer size doesn't match with sample rate, audio recording process risks to not work properly.\n

  @par Record AMR audio format
  This function can record audio signal , compress it with AMR or AMR-WB codec and store it in RTP audio format (refer to RFC4867 to get more information about RTP format for AMR and AMR-WB ).
  Before to start an AMR or AMR-WB audio recording process, application has to set parameters such as codec rate ( refer to #adl_audioAmrCodecRate_e to get available codec rate ), buffer size ( refer to #ADL_AUDIO_AMR_BUFFER_SIZE to get more information ),  mixed voice option ( refer to #ADL_AUDIO_AMR_MIXED_VOICE to get more information about it ).
  According to audio configuration, an audio signal could be compressed either with AMR codec or with AMR-WB codec :
  - for an audio signal with sample rate at 8 KHZ, an AMR codec has to be used.
  - for an audio signal with sample rate at 16 KHZ, an AMR-WB codec has to be used. <b>AMR-WB audio recording is only available on microphone</b>
  Refer to the  <b>AT+SPEAKER</b> command in "AT commands interface guide" to get more information about audio resource configuration.\n
  Buffer size, which has to be allocated, depends on the codec rate selected by application. For each codec rate, a minimal space memory size has to be allocated.
  Buffer size has to be either audio AMR file size or multiple of one 20-ms audio AMR speech frame size, moreover one octet has to be allocated for frame header (this last one depends on codec rate ).\n
  <b>WARNING : 
  - If allocated buffer size doesn't match with codec rate, quality of played audio signal risks to be degraded .
  - when AMR audio file is listened to on voice call and high level IRQ notification has been subscribed with ADL_IRQ_OPTION_AUTO_READ option (refer to adl_irq.h to get more information about this option) and audio buffer is too huge then a network de-registration may occur. In this case ADL_IRQ_OPTION_AUTO_READ option should not be used or audio buffer size should be a small AMR speech frame size
  </b>
  \n

  @param[in]        audioHandle           Handle of the audio resource from which to listen the audio stream (microphone or voice call RX).
  @param[in]        audioFormat           Stream audio format (refer to #adl_audioFormats_e for more information).
  @param[in]        LowLevelIRQHandle     Low level IRQ handle previously returned by IRQ subscription (refer to adl_irqSubscribe() in ADL user guide for more information).
  @param[in]        HighLevelIRQHandle    High level IRQ handle previously returned by IRQ subscription (refer to adl_irqSubscribe() in ADL user guide for more information).
  @param[in]        buffer                contains received decoded DTMF or audio samples.

  @return   Returned value description
  - <b>OK</b>    on success
  - <b>#ADL_RET_ERR_PARAM  </b>            if parameters have an incorrect value.
  - <b>#ADL_RET_ERR_UNKNOWN_HDL </b>       if the provided handle is unknown.
  - <b>#ADL_RET_ERR_BAD_STATE</b>          if an audio stream is listenning or audio signal is playing on the required audio resource.
  - <b>#ADL_RET_ERR_BAD_HDL</b>            if the audio resource is not allowed for audio stream listenning or if interruption handler identifiers are invalid.
  - <b>#ADL_RET_ERR_NOT_SUPPORTED</b>      if the audio resource is not available for audio stream listening.
  - <b>#ADL_RET_ERR_SERVICE_LOCKED</b>     if called from a low level interruption handler.

  @note \n
  - the LowLevelIRQHandle is optionnal is the HighLevelIRQHandle is supplied.
  - the HighLevelIRQHandle is optionnal is the LowLevelIRQHandle is supplied.
  - each time an audio sample or DTMF sequence is detected , an interrupt handler will be notified to require the data. The interrupt identifier will be set to #ADL_IRQ_ID_AUDIO_RX_LISTEN or #ADL_IRQ_ID_AUDIO_TX_LISTEN, according to the resource used to start the stream listen.
  - all audio filters will be deactivated for DTMF listenning and only some audio filters for audio sample listenning (refer to "audio command" chapter in the "AT command Interface Guide" for more information).
  - for audio interruption subscription, #ADL_IRQ_OPTION_POST_ACKNOWLEDGEMENT option is not available.

  @code
  // audio resource handle
  s32 handle;

  // audio stream buffer
  void * StreamBuffer;


  // Low level interruption handler
  bool MyLowLevelIRQHandler ( adl_irqID_e Source, adl_irqNotificationLevel_e Notification Level, adl_irqEventData_t * Data )
  {
      TRACE (( 1, "DTMF received : %c, %c ", StreamBuffer[0], StreamBuffer[1] ));

      return FALSE;
  }

  // audio event call-back function
  void MyAudioEventHandler ( s32 audioHandle, adl_audioEvents_e Event )
  {

      // ...

      return;
  }

  void adl_main ( adl_InitType_e InitType )
  {
      s32 Ret;
      s32 BufferSize;

      // Subscribe to the current microphone
      handle = adl_audioSubscribe ( ADL_AUDIO_MICROPHONE, MyAudioEventHandler , ADL_AUDIO_RESOURCE_OPTION_FORBID_PREEMPTION );

      // Memory allocation
      Ret = adl_audioGetOption ( handle, ADL_AUDIO_DTMF_RAW_STREAM_BUFFER_SIZE, &BufferSize )
      StreamBuffer = adl_memGet( BufferSize );   // release memory after audio stream listening

      // Listen to audio DTMF stream
      Ret = adl_audioStreamListen( handle, ADL_AUDIO_DTMF MyLowLevelIRQHandler, 0, StreamBuffer);
  }
  @endcode

 */
//s32 adl_audioStreamListen (s32 audioHandle, adl_audioFormats_e audioFormat,  s32 LowLevelIRQHandle, s32 HighLevelIRQHandle, void * buffer );


/**
  * @}
  */





/**
  @defgroup adl_audio_public_IFS_op4 Stop
  @ingroup adl_audio_public_IFS

  This function allow a client to stop the playing or listenning on the required audio resource.
  */

/**
  * @addtogroup adl_audio_public_IFS_op4
  * @{
  */


/**
  @par Description:
  This function allows to :
  - stop playing a tone on the current speaker or on the buzzer
  - stop playing a DTMF on the current speaker or on the voice call TX
  - stop playing a melody on the current speaker or on the buzzer
  - stop playing an audio PCM stream on the current speaker or on the voice call TX
  - stop listening to an audio DTMF stream from current microphone or voice call RX
  - stop listening to an audio sample stream from current microphone or voice call RX

  #ADL_AUDIO_EVENT_NORMAL_STOP event will not be sent to application.




  @param[in]        audioHandle     Handle of the audio resource which has to stop its process.

  @return   Returned value description
  - <b>OK</b>                             on success.
  - <b>#ADL_RET_ERR_UNKNOWN_HDL </b>       if the provided handle is unknown.
  - <b>#ADL_RET_ERR_BAD_STATE</b>          if no audio process is running on the required audio resource.
  - <b>#ADL_RET_ERR_SERVICE_LOCKED</b>     if called from a low level interruption handler.

  @code
  // audio resource handle
  s32 handle;

  void adl_main ( adl_InitType_e InitType )
  {
      s32 Ret;

      // Subscribe to the current speaker
      handle = adl_audioSubscribe ( ADL_AUDIO_SPEAKER, MyAudioEventHandler , ADL_AUDIO_RESOURCE_OPTION_FORBID_PREEMPTION );

      // Play a single tone
      Ret = adl_audioTonePlay( handle, 300, -10, 0, 0, 50 );

      // Stop playing the single tone
      Ret = adl_audioStop( handle );

      // unsubscribe to the current speaker
      Ret = adl_audioUnsubscribe ( handle );

  }
  @endcode

 */
s32 adl_audioStop (s32 audioHandle);

/**
  * @}
  */





/**
  @defgroup adl_audio_public_IFS_op5 Set/Get options
  @ingroup adl_audio_public_IFS

  These functions allow a client to set or get options about Audio service.
  */

/**
  * @addtogroup adl_audio_public_IFS_op5
  * @{
  */


/**
  @par Description:
  This function allows to set an audio option according to audio resource and option type specified.
  Severals option types are only readable, so this function can not be used with them (refer to #adl_audioOptionTypes_e for more information).


  @param[in]        audioHandle           Handle of the audio resource.
  @param[in]        audioOption           This parameter defines audio option to set (refer to #adl_audioOptionTypes_e for more information).
  @param[in]        value                 Defines setting value for option.

  @return   Returned value description
  - <b>OK</b>    on success
  - <b>#ADL_RET_ERR_PARAM  </b>            if parameters have an incorrect value.
  - <b>#ADL_RET_ERR_UNKNOWN_HDL </b>       if the provided handle is unknown.
  - <b>#ADL_RET_ERR_NOT_SUPPORTED</b>      if the requested option is associated to a feature not available on the platform.


 */
//s32 adl_audioSetOption (s32 audioHandle, adl_audioOptionTypes_e audioOption,  s32 value);





/**
  @par Description:
  this functions allows to get information about audio service according to audio resource and option type specified.


  @param[in]        audioHandle           Handle of the audio resource.
  @param[in]        audioOption           audio option which wishes to get information (refer to #adl_audioOptionTypes_e for more information).
  @param[out]       Value                 option value according to audio option which has been set.

  @return   Returned value description
  - <b>OK</b>    on success
  - <b>#ADL_RET_ERR_PARAM  </b>            if parameters have an incorrect value.
  - <b>#ADL_RET_ERR_UNKNOWN_HDL </b>       if the provided handle is unknown.
  - <b>#ADL_RET_ERR_NOT_SUPPORTED</b>      if all features associated to the option are not available.

 */
//s32 adl_audioGetOption (s32 audioHandle, adl_audioOptionTypes_e audioOption, s32 * value );


/**
  * @}
  */

#endif




#endif /* ADL_AUDIO_H_ */