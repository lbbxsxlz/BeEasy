libspdm_return_t do_handshake_via_spdm(void)
{
    void *spdm_context;
    libspdm_return_t status;
    uint32_t session_id;
    uint8_t heartbeat_period;
    uint8_t measurement_hash[LIBSPDM_MAX_HASH_SIZE];

    spdm_context = m_spdm_context;
    heartbeat_period = 0;
    
    libspdm_zero_mem(measurement_hash, sizeof(measurement_hash));
    /* Only test KEY_EXCHANGE and FINISH request messages */
    status = libspdm_start_session(spdm_context, false,
                                   m_use_measurement_summary_hash_type,
                                   m_use_slot_id, m_session_policy, &session_id,
                                   &heartbeat_period, measurement_hash);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        printf("libspdm_start_session - %x\n", (uint32_t)status);
        return status;
    }

    return LIBSPDM_SEVERITY_SUCCESS;
}

/*
* These function implements the request and response messages used for provisioning a device with certificate chains.
* Provisioning of Slot 0 should be only done in a secure environment (such as a secure manufacturing environment)
*/
libspdm_return_t do_certificate_provising_via_spdm(uint32_t* session_id)
{
    void *spdm_context;

#if LIBSPDM_ENABLE_CAPABILITY_GET_CSR_CAP
    uint8_t csr_form_get[LIBSPDM_MAX_CSR_SIZE];
    size_t csr_len;
#endif /*LIBSPDM_ENABLE_CAPABILITY_GET_CSR_CAP*/

#if LIBSPDM_ENABLE_SET_CERTIFICATE_CAP
    void *cert_chain_to_set;
    size_t cert_chain_size_to_set;
    uint8_t slot_id;
    bool res;

    cert_chain_to_set = NULL;
    cert_chain_size_to_set = 0;
#endif /*LIBSPDM_ENABLE_SET_CERTIFICATE_CAP*/

    libspdm_return_t status;
    spdm_context = m_spdm_context;

#if LIBSPDM_ENABLE_CAPABILITY_GET_CSR_CAP

    /*get csr*/
    csr_len = LIBSPDM_MAX_CSR_SIZE;
    libspdm_zero_mem(csr_form_get, sizeof(csr_form_get));
    if ((m_exe_session & EXE_SESSION_GET_CSR) != 0) {
        status = libspdm_get_csr(spdm_context, NULL, 0, NULL, 0, NULL, csr_form_get,
                                &csr_len);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            printf("libspdm_get_csr - %x\n",
                    (uint32_t)status);
            return status;
        }
    }
 
#endif /*LIBSPDM_ENABLE_CAPABILITY_GET_CSR_CAP*/

#if LIBSPDM_ENABLE_SET_CERTIFICATE_CAP
    res = libspdm_read_responder_public_certificate_chain(m_use_hash_algo,
                                                        m_use_asym_algo,
                                                        &cert_chain_to_set,
                                                        &cert_chain_size_to_set,
                                                        NULL, NULL);
    if (!res) {
        printf("set certificate :read_responder_public_certificate_chain fail!\n");
        free(cert_chain_to_set);
        return LIBSPDM_STATUS_INVALID_CERT;
    }

    /*set_certificate for slot_id:0 in secure environment*/
    if ((m_exe_session & EXE_SESSION_SET_CERT) != 0) {
        slot_id = 0;
        status = libspdm_set_certificate(spdm_context, slot_id,
                                        cert_chain_to_set, cert_chain_size_to_set, NULL);

        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            printf("libspdm_set_certificate - %x\n",
                (uint32_t)status);
            free(cert_chain_to_set);
            return status;
        }

    }

    /*set_certificate for slot_id:1 in secure session*/
    if (session_id != NULL) {
        if ((m_exe_session & EXE_SESSION_SET_CERT) != 0) {
            slot_id = 1;
            status = libspdm_set_certificate(spdm_context, slot_id,
                                            cert_chain_to_set, cert_chain_size_to_set, session_id);

            if (LIBSPDM_STATUS_IS_ERROR(status)) {
                printf("libspdm_set_certificate - %x\n",
                            (uint32_t)status);
            }

            free(cert_chain_to_set);
            return status;
        }
    }

    free(cert_chain_to_set);
#endif /*LIBSPDM_ENABLE_SET_CERTIFICATE_CAP*/ 
    return LIBSPDM_SEVERITY_SUCCESS;  
}



/* when use --trans NONE, skip secure session  */
    if (m_use_transport_layer == SOCKET_TRANSPORT_TYPE_NONE) {
        if (m_use_version >= SPDM_MESSAGE_VERSION_11) {
            if ((m_exe_session & EXE_SESSION_KEY_EX) != 0) {
                status = do_handshake_via_spdm();
                if (LIBSPDM_STATUS_IS_ERROR(status)) {
                    printf("do_handshake_via_spdm - %x\n",
                        (uint32_t)status);
                    goto done;
                }
            }

            if ((m_exe_session & EXE_SESSION_KEY_EX) != 0) {
                if (m_use_slot_id == 0) {
                    m_use_slot_id =  1;
                    status = do_handshake_via_spdm();
                    if (LIBSPDM_STATUS_IS_ERROR(status)) {
                        printf("do_handshake_via_spdm - %x\n",
                            (uint32_t)status);
                        goto done;
                    }
                }
            }
        }

        if (m_use_version >= SPDM_MESSAGE_VERSION_12) {      
            status = do_certificate_provising_via_spdm(NULL);
            if (LIBSPDM_STATUS_IS_ERROR(status)) {
                    printf("do_certificate_provising_via_spdm - %x\n",
                        (uint32_t)status);
                goto done;
            }
        }
    }
    else {
    }

if (m_use_version >= SPDM_MESSAGE_VERSION_12) {      
        status = do_certificate_provising_via_spdm(&session_id);
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
            printf("do_certificate_provising_via_spdm - %x\n",
                (uint32_t)status);
            return status;
        }
    }

libspdm_return_t do_handshake_via_spdm(void);
libspdm_return_t do_certificate_provising_via_spdm(uint32_t* session_id);

/**
 * Encode an SPDM from a transport layer message.
 *
 * Only for normal SPDM message, it adds the transport layer wrapper.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_id                    Indicates if it is a secured message protected via SPDM session.
 *                                     If *session_id is NULL, it is a normal message.
 *                                     If *session_id is NOT NULL, it is a secured message.
 * @param  is_app_message                 Indicates if it is an APP message or SPDM message.
 * @param  is_requester                  Indicates if it is a requester message.
 * @param  transport_message_size         size in bytes of the transport message data buffer.
 * @param  transport_message             A pointer to a source buffer to store the transport message.
 *                                     For normal message or secured message, it shall point to acquired receiver buffer.
 * @param  message_size                  size in bytes of the message data buffer.
 * @param  message                      A pointer to a destination buffer to store the message.
 *                                     On input, it shall be msg_buf_ptr from receiver buffer.
 *                                     On output, for normal message, it will point to the original receiver buffer.
 *                                     On output, for secured message, it will point to the scratch buffer in spdm_context.
 *
 * @retval LIBSPDM_STATUS_SUCCESS               The message is decoded successfully.
 * @retval LIBSPDM_STATUS_UNSUPPORTED_CAP       The message is invalid.
 **/
libspdm_return_t spdm_transport_none_encode_message(
    void *spdm_context, const uint32_t *session_id, bool is_app_message,
    bool is_requester, size_t message_size, void *message,
    size_t *transport_message_size, void **transport_message)
{
    libspdm_return_t status;

    if (is_app_message && (session_id == NULL)) {
        return LIBSPDM_STATUS_UNSUPPORTED_CAP;
    }

    status = none_encode_message(NULL, message_size, message,
                                transport_message_size,
                                transport_message);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR, "transport_encode_message - %p\n",
                        status));
        return status;
    }

    return LIBSPDM_STATUS_SUCCESS;
}

/**
 * Decode an SPDM from a transport layer message.
 *
 * Only for normal SPDM message, it removes the transport layer wrapper.
 *
 * @param  spdm_context                  A pointer to the SPDM context.
 * @param  session_id                    Indicates if it is a secured message protected via SPDM session.
 *                                     If *session_id is NULL, it is a normal message.
 *                                     If *session_id is NOT NULL, it is a secured message.
 * @param  is_app_message                 Indicates if it is an APP message or SPDM message.
 * @param  is_requester                  Indicates if it is a requester message.
 * @param  transport_message_size         size in bytes of the transport message data buffer.
 * @param  transport_message             A pointer to a source buffer to store the transport message.
 *                                     For normal message or secured message, it shall point to acquired receiver buffer.
 * @param  message_size                  size in bytes of the message data buffer.
 * @param  message                      A pointer to a destination buffer to store the message.
 *                                     On input, it shall be msg_buf_ptr from receiver buffer.
 *                                     On output, for normal message, it will point to the original receiver buffer.
 *                                     On output, for secured message, it will point to the scratch buffer in spdm_context.
 *
 * @retval LIBSPDM_STATUS_SUCCESS               The message is decoded successfully.
 * @retval LIBSPDM_STATUS_UNSUPPORTED_CAP       The message is invalid.
 **/
libspdm_return_t spdm_transport_none_decode_message(
    void *spdm_context, uint32_t **session_id,
    bool *is_app_message, bool is_requester,
    size_t transport_message_size, void *transport_message,
    size_t *message_size, void **message)
{
    
    libspdm_return_t status;

    if ((session_id == NULL) || (is_app_message == NULL)) {
        return LIBSPDM_STATUS_UNSUPPORTED_CAP;
    }
    
    /* get non-secured message*/
    status = none_decode_message(NULL,
                                transport_message_size,
                                transport_message,
                                message_size, message);
    if (LIBSPDM_STATUS_IS_ERROR(status)) {
        LIBSPDM_DEBUG((LIBSPDM_DEBUG_ERROR, "transport_decode_message - %p\n",
                        status));
        return status;
    }

    *session_id = NULL;
    *is_app_message = false;
    return LIBSPDM_STATUS_SUCCESS;
}


