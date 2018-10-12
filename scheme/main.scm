;;;
;;; Entry point for the program
;;;

(load "scheme/util.scm")
(load "scheme/command-reader.scm")
(load "scheme/boot.scm")


(define test-obj (player))

(Game_setCameraTarget test-obj)


(define (update-game)
  (if (Game_keyPressed Key_esc)
      (command-reader))
  (Game_update))


(let main-loop ()
  (cond
   ((not (Game_isRunning))
    '())
   (else
    (update-game)
    (main-loop))))
