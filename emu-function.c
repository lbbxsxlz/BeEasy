/*
* These function implements the request and response messages used for provisioning a device with certificate chains.
* Provisioning of Slot 0 should be only done in a secure environment (such as a secure manufacturing environment)
*/
libspdm_return_t do_certificate_provising_via_spdm(void)
{
    void *spdm_context;

//#if LIBSPDM_ENABLE_CAPABILITY_GET_CSR_CAP
    uint8_t csr_form_get[LIBSPDM_MAX_CSR_SIZE];
    size_t csr_len;
//#endif /*LIBSPDM_ENABLE_CAPABILITY_GET_CSR_CAP*/

#if LIBSPDM_ENABLE_SET_CERTIFICATE_CAP
    void *cert_chain_to_set;
    size_t cert_chain_size_to_set;
    uint8_t slot_id;
    bool res;

    cert_chain_to_set = NULL;
    cert_chain_size_to_set = 0;
#endif /*LIBSPDM_ENABLE_SET_CERTIFICATE_CAP*/

    libspdm_return_t status = LIBSPDM_SEVERITY_SUCCESS;
    spdm_context = m_spdm_context;

//#if LIBSPDM_ENABLE_CAPABILITY_GET_CSR_CAP
    if (m_use_version >= SPDM_MESSAGE_VERSION_12) {
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
    }    
//#endif /*LIBSPDM_ENABLE_CAPABILITY_GET_CSR_CAP*/

#if LIBSPDM_ENABLE_SET_CERTIFICATE_CAP
    if (m_use_version >= SPDM_MESSAGE_VERSION_12) {
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
            }

        }

        free(cert_chain_to_set);
    }
#endif /*LIBSPDM_ENABLE_SET_CERTIFICATE_CAP*/ 
    return status;  
}


/* when use --trans NONE, skip secure session  */
    if (m_use_transport_layer == SOCKET_TRANSPORT_TYPE_NONE) {
        status = do_certificate_provising_via_spdm();
        if (LIBSPDM_STATUS_IS_ERROR(status)) {
                printf("do_session_via_spdm - %x\n",
                       (uint32_t)status);
            goto done;
        }
    }
   else {
   }
