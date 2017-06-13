(add-load-path "." :relative)

(use binary.io)
(use gauche.selector)
(use gauche.uvector)

(use surt.ethernet)
(use surt.network-interface)
(use surt.packet)

(load "ipv6.scm")


(define (u8vector-join a b)
  (list->u8vector
   (append (u8vector->list a) (u8vector->list b))))

(define (send-ns target)
  (define (make-ipv6-packet :key (dst #f) )
    )

  (define (make-icmp6-packet type code payload :optional (reserved 0))
    (let ((hdr (make-u8vector 8 0)))
      (put-u8! hdr 0 type)
      (put-u8! hdr 1 code)
      (put-u16be! hdr 2 0)
      (put-u32be! hdr 4 reserved)
    )
    (make-icmp6-packet 135 0 (ipv6-address->u8vector target))
    ))

(define (ethernet-receive uvec)
  (print :received)
  )

(if #f
    (let ((pcap (make-pcap))
	  (buf  (make-u8vector 1600)))
      (send-ns (string->ipv6-address "fe80::1"))
      (format #t "~a\n" (pcap-inject (make-u8vector 64 18)))
      (print (pcap-get-selectable-fd))
      (while #t
	(pcap-next buf)
	(ethernet-receive buf))))

(define (print-hexdump uvec len)
  (do ((base 0 (+ base 16)))
      ((>= base len))
    (format #t "~8,'0x  " base)
    (do ((i 0 (inc! i)))
	((or (= i 16)
	     (>= (+ base i) len)))
      (format #t "~2,'0x " (u8vector-ref uvec (+ base i)))
      (if (= i 7)
	  (format #t " ")))
    (print)))

(define (tcpdump)
  (let ((pcap (make-pcap))
	(uvec (make-u8vector 2000)))
    (while #t
      (let1 n (pcap-next uvec)
	(print-hexdump uvec n)))))



;;; <ip-stack>

(define-class <ip-stack> ()
  ((interfaces :init-form (list))
   (selector   :init-form (make <selector>))
   ))

(define-method ip-stack-get-interface ((ip-stack <ip-stack>) ifname)
  (any (lambda (netif)
	 (string=? ifname (network-interface-name netif)))
       (ref ip-stack 'interfaces)))

(define (make-ip-stack)
  (make <ip-stack>))

(define (ip-stack-add-ethernet ip-stack ifname)
  (let1 eth (make-ethernet ip-stack ifname)
    (slot-push! ip-stack 'interfaces eth)
    eth))

(define (main args)
  (let1 host (make-ip-stack)
    (let1 eth (ip-stack-add-ethernet host "en0")
      (ip-stack-get-interface host "eth0")
      )))
