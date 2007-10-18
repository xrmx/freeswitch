/* 
 * FreeSWITCH Modular Media Switching Software Library / Soft-Switch Application
 * Copyright (C) 2005/2006, Anthony Minessale II <anthmct@yahoo.com>
 *
 * Version: MPL 1.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is FreeSWITCH Modular Media Switching Software Library / Soft-Switch Application
 *
 * The Initial Developer of the Original Code is
 * Anthony Minessale II <anthmct@yahoo.com>
 * Portions created by the Initial Developer are Copyright (C)
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 * 
 * Anthony Minessale II <anthmct@yahoo.com>
 * Michael Jerris <mike@jerris.com>
 * Paul D. Tinsley <pdt at jackhammer.org>
 *
 *
 * switch_core_codec.c -- Main Core Library (codec functions)
 *
 */
#include <switch.h>
#include "private/switch_core_pvt.h"

SWITCH_DECLARE(switch_status_t) switch_core_session_set_read_codec(switch_core_session_t *session, switch_codec_t *codec)
{
	switch_event_t *event;

	assert(session != NULL);

	if (switch_event_create(&event, SWITCH_EVENT_CODEC) == SWITCH_STATUS_SUCCESS) {
		switch_channel_event_set_data(session->channel, event);
		switch_event_add_header(event, SWITCH_STACK_BOTTOM, "channel-read-codec-name", "%s", codec->implementation->iananame);
		switch_event_add_header(event, SWITCH_STACK_BOTTOM, "channel-read-codec-rate", "%d", codec->implementation->samples_per_second);
		switch_event_fire(&event);
	}

	session->read_codec = codec;
	session->raw_read_frame.codec = session->read_codec;
	session->raw_write_frame.codec = session->read_codec;
	session->enc_read_frame.codec = session->read_codec;
	session->enc_write_frame.codec = session->read_codec;

	return SWITCH_STATUS_SUCCESS;
}

SWITCH_DECLARE(switch_codec_t *) switch_core_session_get_read_codec(switch_core_session_t *session)
{
	return session->read_codec;
}

SWITCH_DECLARE(switch_status_t) switch_core_session_set_write_codec(switch_core_session_t *session, switch_codec_t *codec)
{
	switch_event_t *event;
	assert(session != NULL);

	if (switch_event_create(&event, SWITCH_EVENT_CODEC) == SWITCH_STATUS_SUCCESS) {
		switch_channel_event_set_data(session->channel, event);
		switch_event_add_header(event, SWITCH_STACK_BOTTOM, "channel-write-codec-name", "%s", codec->implementation->iananame);
		switch_event_add_header(event, SWITCH_STACK_BOTTOM, "channel-write-codec-rate", "%d", codec->implementation->samples_per_second);
		switch_event_fire(&event);
	}

	session->write_codec = codec;
	return SWITCH_STATUS_SUCCESS;
}

SWITCH_DECLARE(switch_codec_t *) switch_core_session_get_write_codec(switch_core_session_t *session)
{
	return session->write_codec;
}

SWITCH_DECLARE(switch_status_t) switch_core_codec_init(switch_codec_t *codec, char *codec_name, char *fmtp,
													   uint32_t rate, int ms, int channels, uint32_t flags,
													   const switch_codec_settings_t *codec_settings, switch_memory_pool_t *pool)
{
	const switch_codec_interface_t *codec_interface;
	const switch_codec_implementation_t *iptr, *implementation = NULL;
	char *mode = fmtp;

	assert(codec != NULL);
	assert(codec_name != NULL);

	memset(codec, 0, sizeof(*codec));


	if (channels == 2) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Stereo is not currently supported. please downsample audio source to mono.\n");
		return SWITCH_STATUS_GENERR;
	}

	if ((codec_interface = switch_loadable_module_get_codec_interface(codec_name)) == 0) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "invalid codec %s!\n", codec_name);
		return SWITCH_STATUS_GENERR;
	}

	if (!strcasecmp(codec_name, "ilbc") && mode && strncasecmp(mode, "mode=", 5)) {
		int mms;
		mode += 5;
		if (mode) {
			mms = atoi(mode);
			if (mms > 0 && mms < 120) {
				ms = mms;
			}
		}
	}

	/* If no specific codec interval is requested opt for 20ms above all else because lots of stuff assumes it */
	if (!ms) {
		for (iptr = codec_interface->implementations; iptr; iptr = iptr->next) {
			if ((!rate || rate == iptr->samples_per_second) &&
				(20 == (iptr->microseconds_per_frame / 1000)) && (!channels || channels == iptr->number_of_channels)) {
				implementation = iptr;
				goto found;
			}
		}
	}

	/* Either looking for a specific interval or there was no interval specified and there wasn't one @20ms available */
	for (iptr = codec_interface->implementations; iptr; iptr = iptr->next) {
		if ((!rate || rate == iptr->samples_per_second) &&
			(!ms || ms == (iptr->microseconds_per_frame / 1000)) && (!channels || channels == iptr->number_of_channels)) {
			implementation = iptr;
			break;
		}
	}

  found:

	if (implementation) {
		switch_status_t status;
		codec->codec_interface = codec_interface;
		codec->implementation = implementation;
		codec->flags = flags;

		if (pool) {
			codec->memory_pool = pool;
		} else {
			if ((status = switch_core_new_memory_pool(&codec->memory_pool)) != SWITCH_STATUS_SUCCESS) {
				return status;
			}
			switch_set_flag(codec, SWITCH_CODEC_FLAG_FREE_POOL);
		}

		if (fmtp) {
			codec->fmtp_in = switch_core_strdup(codec->memory_pool, fmtp);
		}

		implementation->init(codec, flags, codec_settings);

		return SWITCH_STATUS_SUCCESS;
	} else {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_WARNING, "Codec %s Exists but not at the desired implementation. %dhz %dms\n", codec_name, rate,
						  ms);
	}

	return SWITCH_STATUS_NOTIMPL;

}

SWITCH_DECLARE(switch_status_t) switch_core_codec_encode(switch_codec_t *codec,
														 switch_codec_t *other_codec,
														 void *decoded_data,
														 uint32_t decoded_data_len,
														 uint32_t decoded_rate,
														 void *encoded_data, uint32_t * encoded_data_len, uint32_t * encoded_rate, unsigned int *flag)
{
	assert(codec != NULL);
	assert(encoded_data != NULL);
	assert(decoded_data != NULL);

	if (!codec->implementation) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Codec is not initilized!\n");
		return SWITCH_STATUS_GENERR;
	}

	if (!switch_test_flag(codec, SWITCH_CODEC_FLAG_ENCODE)) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Codec's encoder is not initilized!\n");
		return SWITCH_STATUS_GENERR;
	}


	return codec->implementation->encode(codec, other_codec, decoded_data, decoded_data_len, decoded_rate, encoded_data, encoded_data_len, encoded_rate,
										 flag);

}

SWITCH_DECLARE(switch_status_t) switch_core_codec_decode(switch_codec_t *codec,
														 switch_codec_t *other_codec,
														 void *encoded_data,
														 uint32_t encoded_data_len,
														 uint32_t encoded_rate,
														 void *decoded_data, uint32_t * decoded_data_len, uint32_t * decoded_rate, unsigned int *flag)
{

	assert(codec != NULL);
	assert(encoded_data != NULL);
	assert(decoded_data != NULL);



	if (!codec->implementation) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Codec is not initilized!\n");
		return SWITCH_STATUS_GENERR;
	}

	if (!switch_test_flag(codec, SWITCH_CODEC_FLAG_DECODE)) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Codec's decoder is not initilized!\n");
		return SWITCH_STATUS_GENERR;
	}


	return codec->implementation->decode(codec, other_codec, encoded_data, encoded_data_len, encoded_rate, decoded_data, decoded_data_len, decoded_rate,
										 flag);

}

SWITCH_DECLARE(switch_status_t) switch_core_codec_destroy(switch_codec_t *codec)
{
	assert(codec != NULL);

	if (!codec->implementation) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_WARNING, "Codec is not initilized!\n");
		return SWITCH_STATUS_GENERR;
	}

	codec->implementation->destroy(codec);

	if (switch_test_flag(codec, SWITCH_CODEC_FLAG_FREE_POOL)) {
		switch_core_destroy_memory_pool(&codec->memory_pool);
	}

	return SWITCH_STATUS_SUCCESS;
}
