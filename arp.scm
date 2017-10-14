(define-module arp
  (use gauche.uvector)
  (export make-arp-table
	  arp-table-add))

(select-module arp)

(define-class <arp-table> ()
  ((alist :init-value '())))

(define (make-arp-table)
  (make <arp-table>))

(define (arp-table-find-by-ipv4-address arptab ipaddr)
  (let1 p (assoc (slot-ref arptab 'alist))
    (if p
	(cdr p)
	#f)))

(define (arp-table-find-by-mac-address arptab mac)
  (let1 p (rassoc (slot-ref arptab 'alist))
    (if p
	(cdr p)
	#f)))

(define (arp-table-add arptab ipaddr macaddr)
  (slot-set! arptab 'alist
	     (acons ipaddr macaddr (slot-ref arptabl 'alist))))

(define (arp-table->list arptab)
  (slot-ref arptab 'alist))
