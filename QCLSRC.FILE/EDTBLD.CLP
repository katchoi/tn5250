000100070907             PGM        PARM(&L &N &A)                                          
000200070907             DCL        VAR(&F) TYPE(*CHAR) LEN(10)                             
000300070907             DCL        VAR(&L) TYPE(*CHAR) LEN(10)                             
000400070907             DCL        VAR(&N) TYPE(*CHAR) LEN(10)                             
000500070907             DCL        VAR(&A) TYPE(*CHAR) LEN(10)                             
000600070907                                                                                
000700070907             IF         COND(&A *EQ 'C') THEN(CHGVAR VAR(&F) +                  
000800070907                          VALUE(QBLDC))                                         
000900070907             IF         COND(&A *EQ 'CL') THEN(CHGVAR VAR(&F) +                 
001000070907                          VALUE(QBLDCL))                                        
001100070907             IF         COND(&A *EQ 'CLLE') THEN(CHGVAR VAR(&F) +               
001200070907                          VALUE(QBLDCLLE))                                      
001300070907             IF         COND(&A *EQ 'CMD') THEN(CHGVAR VAR(&F) +                
001400070907                          VALUE(QBLDCMD))                                       
001500070907             IF         COND(&A *EQ 'PNLGRP') THEN(CHGVAR VAR(&F) +             
001600070907                          VALUE(QBLDPNLSRC))                                    
001700070907             CHKOBJ     OBJ(&L/&F) OBJTYPE(*FILE) MBR(&N)                       
001800070907             MONMSG     MSGID(CPF0000) EXEC(GOTO CMDLBL(ENDPGM))                
001900070907             STRSEU     SRCFILE(&L/&F) SRCMBR(&N)                               
002000070907 ENDPGM:     ENDPGM                                                             
