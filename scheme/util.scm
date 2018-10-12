;;;
;;; Useful misc. stuff not related specifically to anything else
;;;


(define (->string object)
  (define port (open-output-string))
  (write object port)
  (get-output-string port))


(define (int-list->string command)
  (define expr-len (length command))
  (define str (make-string expr-len))
  (let populate ((index (- expr-len 1)) (char-list command))
    (cond
     ((eq? index -1) str)
     (else
      (string-set! str index (integer->char (car char-list)))
      (populate (- index 1) (cdr char-list))))))
