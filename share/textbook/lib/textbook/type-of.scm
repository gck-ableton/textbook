;; Copyright (c) 2017 by Gregor Klinke
;; All rights reserved.

(define (type-of object)
  (cond
   ((pair? object) 'pair)
   ((vector? object) 'vector)
   ((symbol? object) 'symbol)
   ((boolean? object) 'boolean)
   ((char? object) 'char)
   ((integer? object) 'integer)
   ((rational? object) 'rational)
   ((real? object) 'real)
   ((complex? object) 'complex)
   ((string? object) 'string)
   ((null? object) 'null)
   ((eof-object? object) 'eof-object)
   ((keyword? object) 'keyword)
  (else 'unknown)))
