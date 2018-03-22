(use gauche.test)
(use ipv4)
(use mac-address)

(test-start "arp")
(load "arp")
(import arp)

(test-module 'arp)

(test-section "arp table")

(let ((t    (make-arp-table))
      (ip0  (string->ipv4-address "192.168.0.1"))
      (mac0 (string->mac-address  "02:01:02:03:04:05")))

  (arp-table-add! t ip0 mac0)
  (test* "arp table add" mac0 (arp-table-get t ip0))

  (arp-table-delete! t ip0)
  (test* "arp table delete" #f (arp-table-get t ip0))

  )

(test-end)
