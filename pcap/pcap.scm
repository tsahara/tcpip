;;;
;;; pcap
;;;

(define-module pcap
  (export test-pcap ;; dummy
	  <pcap>
	  make-pcap
	  pcap-inject
	  pcap-next
	  pcap-get-selectable-fd
          )
  )
(select-module pcap)

;; Loads extension
(dynamic-load "pcap")

;;
;; Put your Scheme definitions here
;;



