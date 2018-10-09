;;;
;;; Entry point for the program
;;;


(define test-obj (Game_makeObject))
(Object_setPosition test-obj -500.0 0.0)


(define (update-game)
  (Game_update))


(let main-loop ()
  (cond
   ((not (Game_isRunning))
    '())
   (else
    (update-game)
    (main-loop))))
