(use gauche.test)
(use ipv4)
(use macaddr)

(test-start "arp")
(load "arp")
(import arp)


(test-module 'arp)

(test-section "arp table")

(let ((t    (make-arp-table))
      (ip0  (string->ipv4-address "192.168.0.1"))
      (mac0 (string->mac-address  "02:01:02:03:04:05")))
  (arp-table-add t ip0 mac0)

  (test* "arp table add" mac0 (arp-table-find-by-ipv4-address t ip0)))

(test-end)
