;;;
;;; Entry point for the program
;;;

(load "repl.scm")

(define test-txtr (Game_createTexture "Sprite-0001.png"))
(display test-txtr)

(define test-obj (Game_makeObject))
(Object_setFace test-obj)
(Object_setShadow test-obj)

(Game_setCameraTarget test-obj)


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
