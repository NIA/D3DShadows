vs_1_1
dcl_position v0
dcl_texcoord v2

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; c0 - c3 is view matrix           ;;
;; c4 - c7 is 1st bone matrix       ;;
;; c8 - c11 is 2nd bone matrix      ;;
;; c17 is point light position      ;;
;; c27 - c30 is pos.*rot. matrix    ;;
;; c31-c34 is shadow proj. matrix   ;;
;; c35 are shadow attenuat. consts  ;;
;;                                  ;;
;; c100 is constant 0.0f            ;;
;; c111 is constant 1.0f            ;;
;;                                  ;;
; !r3  is transformed vertex        ;;
; !r10 is result color              ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

def c100, 0.0, 0.0, 0.0, 0.0
def c111, 1.0, 1.0, 1.0, 1.0

;;;;;;;;;;;;;;;;;;;;;; Skinning ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; - - - - - - - - - -  position  - - - - - - - - - - - - - -;
m4x4 r1, v0, c4
mul r1.xyz, r1.xyz, v2.x            ; first bone
m4x4 r0, v0, c8
mad r0.xyz, r0.xyz, v2.y, r1.xyz    ; second bone
m4x4 r1, r0, c27                    ; position and rotation
m4x4 r3, r1, c31  ; projection to plane
rcp r7, r3
mul r3, r3, r7.w

; - - - - - - - - - - - color - - - - - - - - - - - - - - - ;
; calculating direction vector
add r11, c17, -r3       ; r11 = position(point) - position(vertex) = AC
dp3 r4, r11, r11        ; r4 = distance**2 = |AC|^2
rsq r7, r4              ; r7 = 1/distance
; calculating attenuation
dst r2, r4, r7          ; r2 = (1, d, d**2, 1/d)
dp3 r0, r2, c35         ; r0 = (a + b*d + c*d**2)
rcp r0, r0              ; r0 = attenuation coef
mov r10, c100           ; r10 = black
mul r10.a, c111.a, r0.a ; r10.alpha = 1 * attenuation
; - - - - - - - - - - checking - - - - - - - - - - - - - - - ;
; A - on the plane, B - vertex, C - light source
add r5, r1,  -r3        ; r5 = AB
dp3 r5, r5, r5          ; r5 = |AB|^2
add r6, c17, -r1        ; r6 = BC
dp3 r6, r6, r6          ; r6 = |BC|^2

sge r8, r4, r5          ; |AC| > |AB|
mul r10.a, r10.a, r8.x
sge r8, r4, r6          ; |AC| > |BC|
mul r10.a, r10.a, r8.x

;;;;;;;;;;;;;;;;;;;;;;;; Results ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
m4x4 oPos, r3, c0
mov oD0, r10
