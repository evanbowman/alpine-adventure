;;;
;;; Entry point for the program
;;;


(define test-obj (Game_makeObject))
(Object_move test-obj -500.0 0.0)


(define (update-game)
  (cond
   ((Game_keyPressed Key_esc) '())
   (else (Game_update))))


(let main-loop ()
  (cond
   ((not (Game_isRunning))
    '())
   (else
    (update-game)
    (main-loop))))
