vs_1_1
dcl_position v0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; c0 - c3 is view matrix           ;;
;; c17 is point light position      ;;
;; c18 are attenuation constants    ;;
;; c27 - c30 is pos.*rot. matrix    ;;
;; c31 - c34 is projection matrix   ;;
;;                                  ;;
;; c100 is constant 0.0f            ;;
;; c111 is constant 1.0f            ;;
;;                                  ;;
; !r1  is transformed vertex        ;;
;; r2  is projected vertex          ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

def c100, 0.0, 0.0, 0.0, 0.0
def c111, 1.0, 1.0, 1.0, 1.0

m4x4 r1, v0, c27  ; position and rotation
m4x4 r2, r1, c31  ; projection to plane
;mov r2, r1

;;;;;;;;;;;;;;;;;;;;;;;; Results ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
m4x4 oPos, r2, c0
mov oD0, c100     ; black color
