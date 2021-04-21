# BB84-Protocol

This is an implementation in C of the theoretically secure BB84 quantum key distribution protocol first conceived in 1984 by Charles H. Bennet and Gilles Brassard. (Source: https://arxiv.org/pdf/2003.06557.pdf )

Symmetrical cryptography algorithms rely on classical channels such as the internet to transmit a secure key which is to be used for encoding and decoding the message. This way of transmission is vulnerable to eavesdroppers (Eve) who may intercept the key without even the sender (Alice) and recipient (Bob) knowing the existence of the eavesdropper.

BB84 symmentrical key distribution protocol exploits quantum properties of uncertainty principle and no cloning principle to securely transmit a private key which in principle cannot be intercepted by an eavesdropper without a high probability of changing the transmission in such a way as to be detected. The sender (Alice) and recepient (Bob) are assured of a private key which no one else knows and which does not require Alice and Bob to share any secret information beforehand.
