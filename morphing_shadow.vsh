vs_1_1
dcl_position v0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; c0 - c3 is view matrix           ;;
;; c4  is FINAL_RADIUS              ;;
;; c5  is t                         ;;
;; c27 - c30 is pos.*rot. matrix    ;;
;;                                  ;;
;; c100 is constant 0.0f            ;;
;; c111 is constant 1.0f            ;;
;;                                  ;;
; !r1  is transformed vertex        ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

def c100, 0.0, 0.0, 0.0, 0.0
def c111, 1.0, 1.0, 1.0, 1.0 ;constant one

;;;;;;;;;;;;;;;;;;;;;; Morphing ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; - - - - - - - - - -  position  - - - - - - - - - - - - - -;
dp3 r0, v0, v0
rsq r0, r0              ; r0 = 1/|r|

mov r2, c111            ; r2 = 1.0f
mad r1, r0, c4, -r2    ; r1 = ((FINAL_RADIUS/r0) - 1)
mad r1, r1, c5, r2     ; r1 = 1+t*((FINAL_RADIUS/r0) - 1)

mul r2.xyz, v0.xyz, r1.x

m4x4 r1, r2, c27        ; position and rotation
m4x4 r2, r1, c31  ; projection to plane

;;;;;;;;;;;;;;;;;;;;;;;; Results ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
m4x4 oPos, r2, c0
mov oD0, c100     ; black color
