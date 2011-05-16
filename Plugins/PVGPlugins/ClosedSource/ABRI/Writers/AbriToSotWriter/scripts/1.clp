(deffacts point-0
	(point0 stope12)
)
(defrule point-0
	(point0 ?stopeId)
	=>
	(printout t "point-0  " ?stopeId crlf)
	(assert (startingPoint ?stopeId))
)
(defrule starting-point
	(startingPoint ?stopeId)
	(neighbors ?stopeId ?neighbor 0)
	=>
	(printout t "starting-point  " ?neighbor " pppp " ?stopeId crlf)
	(assert (startingPoint ?neighbor))
	(assert (received (printDep ?stopeId ?neighbor)))
)
(defrule starting-point2
	(startingPoint ?stopeId)
	(neighbors ?stopeId ?neighbor 2)
	=>
	(printout t "starting-point  2 " ?neighbor " pppp " ?stopeId crlf)
	(assert (startingPoint ?neighbor))
	(assert (received (printDep ?stopeId ?neighbor)))
)
(defrule create-dependencies
	(startingPoint ?stopeId)
	(neighbors ?stopeId ?neighbor 1)
	=>
	(assert (dependency ?stopeId ?neighbor))
	(printout t "dependency  " ?stopeId " " ?neighbor crlf)
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