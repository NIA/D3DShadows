vs_1_1
dcl_position v0
dcl_color v1
dcl_normal v3

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; c0 - c3 is view matrix           ;;
;; c4  is FINAL_RADIUS              ;;
;; c5  is t                         ;;
;; c14 is diffuse coefficient       ;;
;; c15 is ambient light color       ;;
;; c16 is point light color         ;;
;; c17 is point light position      ;;
;; c18 are attenuation constants    ;;
;; c19 is specular coefficient      ;;
;; c20 is specular constant 'f'     ;;
;; c21 is eye position              ;;
;; c27 - c30 is pos.*rot. matrix    ;;
;;                                  ;;
;; c100 is constant 0.0f            ;;
;; c111 is constant 1.0f            ;;
;;                                  ;;
; ?r0  is attenuation               ;;
; !r1  is transformed vertex        ;;
;; r2  is r (for specular)          ;;
;; r3  is temp                      ;;
;; r4  is light intensity           ;;
; !r5  is cos(theta)                ;;
; !r6  is result color              ;;
;; r7  is temp                      ;;
;; r8  is cos(phi) (specular)       ;;
; !r9  is normalized eye (v)        ;;
; !r10 is transformed normal        ;;
;; r11 is direction vector          ;;
;;                                  ;;
;; r0 is 1/current radius           ;;
;; r1 is a quotient                 ;;
;; r2 is a vertex after morphing    ;;
;;                                  ;;
; ?r0  is attenuation               ;;
; !r1  is transformed vertex        ;;
;; r2  is r (for specular)          ;;
;; r3  is temp                      ;;
;; r4  is light intensity           ;;
; !r5  is cos(theta)                ;;
; !r6  is result color              ;;
;; r7  is temp                      ;;
;; r8  is cos(phi) (specular)       ;;
; !r9  is normalized eye (v)        ;;
; !r10 is transformed normal        ;;
;; r11 is direction vector          ;;
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
; - - - - - - - - - -  normals  - - - - - - - - - - - - - - ;
mul r3, v0, r0.x         ; r3 is normalized position and is final normal
mov r3.w, c100.w         ; ... and make it a vector, not a point
add r4, r3, -v3         ; r4 = (r/|r| - n)
mad r4, r4, c5.x, v3   ; r4 = n + (r/|r| - n)*t
dp3 r0, r4, r4          ; r0 = |r4|**2
rsq r7, r0              ; r7 = 1/|r4|
mul r4, r4, r7.x        ; normalized r4
m4x4 r10, r4, c27       ; rotation


; calculating normalized v
add r9, c21, -r1       ; r9 = position(eye) - position(vertex)
dp3 r0, r9, r9         ; r0 = distance**2
rsq r7, r0             ; r7 = 1/distance
mul r9, r9, r7.x       ; normalize r9
;;;;;;;;;;;;;;;;;;;;;;;; Point ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; calculating normalized direction vector
add r11, c17, -r1       ; r11 = position(point) - position(vertex)
dp3 r2, r11, r11        ; r2 = distance**2
rsq r7, r2              ; r7 = 1/distance
mul r11, r11, r7.x      ; normalize r11
; calculating cos(theta)
dp3 r5, r11, r10        ; r5 = cos(theta)
; calculating attenuation
dst r2, r2, r7          ; r2 = (1, d, d**2, 1/d)
dp3 r0, r2, c18         ; r0 = (a + b*d + c*d**2)
rcp r0, r0              ; r0 = attenuation coef
; - - - - - - - - - - - diffuse - - - - - - - - - - - - - - ;
mul r4, c16, r5.x       ; r4 = I(point)*cos(theta)
mul r4, r4, c14.x        ; r4 *= coef(diffuse)
mul r4, r4, r0.x        ; r4 *= attenuation

max r6, r4, c100        ; if some color comp. < 0 => make it == 0
; - - - - - - - - - - - specular - - - - - - - - - - - - - -;
; calculating r:
mul r2, r10, r5.x   ; r2 = (l, n)*n
add r2, r2, r2      ; r2 = 2*(l, n)*n
add r2, r2, -r11    ; r2 = 2*(l, n)*n - l
; calculating cos(phi)**f
dp3 r8, r2, r9          ; r8 = cos(phi)
max r8, r8, c100        ; if cos < 0, let it = 0
mov r7.y, r8.x
mov r7.w, c20.x
lit r8, r7              ; r8.z = cos(phi)**f

mul r4, c16, r8.z       ; r4 = I(point)*cos(phi)**f
mul r4, r4, c19.x       ; r4 *= coef(specular)
mul r4, r4, r0.x        ; r4 *= attenuation

max r4, r4, c100        ; if some color comp. < 0 => make it == 0
add r6, r6, r4

;;;;;;;;;;;;;;;;;;;;;;; Ambient ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
add r6, r6, c15         ; r6 += I(ambient)

;;;;;;;;;;;;;;;;;;;;;;;; Results ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
m4x4 oPos, r1, c0
mov oD0, v1
mul oD0, v1, r6
