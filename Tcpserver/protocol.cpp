#include "protocol.h"



PDU *mk_pdu(uint uiMsgLen)
{
    uint uiPDULen = sizeof(PDU) + uiMsgLen;
    PDU *pdu = (PDU*)malloc(uiPDULen);
    if(pdu == nullptr){
        exit(EXIT_FAILURE);
    }
    pdu->uiMsgLen = uiMsgLen;
    pdu->uiPDULen = uiPDULen;
    return pdu;
}
