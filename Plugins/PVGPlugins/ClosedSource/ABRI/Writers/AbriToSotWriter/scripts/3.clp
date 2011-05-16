;;;*************i will iterate in the positive direction first
(defrule starting-point1
	(startingPoint ?stopeId)
	(neighbors ?stopeId ?neighbor 0)
	(not (exists (dependency-done ?stopeId ?neighbor 0)))
	=> 
		(printout t "starting-point  " ?neighbor " pppp " ?stopeId crlf)
		(assert (startingPoint ?neighbor))
		(assert (received (printDep ?stopeId ?neighbor)))
		(assert (dependency-done ?stopeId ?neighbor 0))
)

;;;*************i will iterate in the negative X direction started by the starting stope
(defrule starting-point1-reverse
	(startingPoint ?stopeId)
	(neighbors ?neighbor ?stopeId 0)
	(not (exists (dependency-done ?neighbor ?stopeId 0)))
	=> 
		(printout t "starting-point  " ?neighbor " pppp " ?stopeId crlf)
		(assert (startingPoint ?neighbor))
		(assert (received (printDep ?neighbor ?stopeId)))
		(assert (dependency-done ?neighbor ?stopeId 0))
)

(defrule starting-point2
	(startingPoint ?stopeId)
	(neighbors ?stopeId ?neighbor 2)
	(not (exists (dependency-done ?stopeId ?neighbor 2)))
	=>
	(printout t "starting-point  2 " ?neighbor " pppp " ?stopeId crlf)
	(assert (startingPoint ?neighbor))
	(assert (received (printDep ?stopeId ?neighbor)))
	(assert (dependency-done ?stopeId ?neighbor 2))
)

;;;*************i will iterate in the negative Y direction started by the starting stope
(defrule starting-point2-reverse
	(startingPoint ?stopeId)
	(neighbors ?neighbor ?stopeId 2)
	(not (exists (dependency-done ?neighbor ?stopeId 2)))
	=>
	(printout t "starting-point  2 " ?neighbor " pppp " ?stopeId crlf)
	(assert (startingPoint ?neighbor))
	(assert (received (printDep ?neighbor ?stopeId)))
	(assert (dependency-done ?neighbor ?stopeId 2))
)

;;;*************iteration from the XZ layer to the positive y direction (we need to do the same reverse)
(defrule create-dependencies
	(startingPoint ?stopeId)
	(neighbors ?stopeId ?neighbor 1)
	=>
	(assert (dependency ?stopeId ?neighbor))
	(printout t "dependency  " ?stopeId " " ?neighbor crlf)
	;;i add the next line to make a dependency between the fisrt layer and the next layers in y direction
	(assert (received (printDep ?stopeId ?neighbor)))
)

(defrule create-dependencies2
	(dependency ?stopeId ?neighbor)
	(neighbors ?neighbor ?neighbor2 1)
	=>
	(assert (dependency ?neighbor ?neighbor2))
	(printout t "dependency  " ?neighbor " " ?neighbor2 crlf)
	(assert (received (printDep ?neighbor ?neighbor2)))
)

(defrule print-facts
=>
(facts)
)