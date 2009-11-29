vs_1_1
dcl_position v0
dcl_color v1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; c0 - c3 is view matrix           ;;
;; c4 is radius                     ;;
;; c16 is point light color         ;;
;; c17 is point light position      ;;
;;                                  ;;
;; c111 is constant 1.0f            ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

def c111, 1.0, 1.0, 1.0, 1.0

; normalize v0
mov r0, v0
dp3 r2, r0, r0
rsq r7, r2
mul r0, r0, r7.x
; multiply to radius
mul r0, r0, c4.x
; move
add r0, r0, c17
mov r0.w, c111.w
; results
m4x4 oPos, r0, c0
mov oD0, c16
