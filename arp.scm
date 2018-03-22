(define-module arp
  (use mac-address)
  (use gauche.uvector)
  (use srfi-69)
  (export make-arp-table
	  arp-table-add!
	  arp-table-delete!
	  arp-table-get
	  ))

(select-module arp)

(define-class <arp-table> ()
  ((ht :init-form (make-hash-table mac-address-cmp mac-address-hash))))

(define (make-arp-table)
  (make <arp-table>))

(define (arp-table-get arptab ip)
  (hash-table-ref/default (ref arptab 'ht) ip #f))

(define (arp-table-add! arptab ip mac)
  (hash-table-set! (ref arptab 'ht) ip mac))

(define (arp-table-delete! arptab ip)
  (hash-table-delete! (ref arptab 'ht) ip))


(define (arp-table-find-by-mac-address arptab mac)
  (let1 p (rassoc mac (slot-ref arptab 'alist))
    (if p
	(cdr p)
	#f)))

(define (arp-table->list arptab)
  (slot-ref arptab 'alist))
