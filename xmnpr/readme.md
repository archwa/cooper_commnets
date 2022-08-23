# xmnpr

A lossless transmission protocol implemented in Python.

## Description

The ECE303 API provides an unreliable channel over which the protocol must transmit.
`send.py` and `receive.py` work in coordination to ensure lossless transmission of data.
If any packets are dropped, corrupted, sent out of order, etc., `xmnpr` guarantees retransmission until success.

## Dependencies

* Python 2.7
* ECE303 API (included within `lib/api/`

## Usage

receive:
```bash
PYTHONPATH=lib/:lib/api/2018/ python2 receive.py > RECEIVE_OUTPUT
```

send:
```bash
PYTHONPATH=lib/:lib/api/2018/ python2 send.py > SEND_LOG < SEND_INPUT
```

fork receive, then send (full demo):
```bash
make run
```
