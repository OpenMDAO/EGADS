	program main
c
	include 'egads.inc'
c
	pointer      (psenses, senses), (pbody, bodies)
	pointer      (pfaces, faces),   (pedges, edges)
	pointer      (pnodes, nodes),   (pdum,   dummy)
        integer      oclass, senses(*)
	integer*8    null, context8, model8, geom8, ref, prev, next
	integer*8    bodies(*), faces(*), edges(*), nodes(*)
	integer*8    dummy(*)
	real*8       t, size, large, sum, eres(18), xyz(4)
        real*8       limits(4), box(6), uv(2), result(18)
	character*80 name
	data         null/0/
c
c       initialize
c
	istat = IG_open(context8)
	write(*,*) 'IG_open = ', istat
c
	write(*,*) 'Enter Filename:'
	read(*,'(a)') name
c
c       load the file
c
	istat = IG_loadModel(context8, 0, name, model8)
	write(*,*) 'IG_loadModel = ', istat
        write(*,*) ' '
c
c       get the bodies
c
        istat = IG_getTopology(model8, geom8, oclass, mtype, limits, 
     &                         nbodies, pbody, psenses)
	if (istat .NE. 0) then
	  write(*,*) 'IG_getTopology =', istat
	  call exit(1)
	endif
	write(*,*) 'nBodies = ', nbodies
c
c	examine a body at a time
c
	do 1 i = 1, nbodies
	  istat = IG_getBodyTopos(bodies(i), null, FACE, nfaces, pfaces)
	  if (istat .NE. 0) then
            write(*,*) ' Body', i, ' IG_getBodyTopos =', istat
	    go to 1
	  endif
	  istat = IG_getBoundingBox(bodies(i), box)
	  if (istat .NE. 0) then
            write(*,*) ' Body', i, ' IG_getBoundingBox =', istat
	    go to 1
	  endif
          size = dsqrt((box(4)-box(1))*(box(4)-box(1)) +
     &                 (box(5)-box(2))*(box(5)-box(2)) +
     &                 (box(6)-box(3))*(box(6)-box(3)))
	  write(*,*) 'Body = ', i, '  nFaces = ', nfaces,
     &               '  size = ', size
c
c	  look at each Face
          sum = 0.0
	  do 2 j = 1, nfaces
c
c           find largest edge closure
            istat = IG_getBodyTopos(bodies(i), faces(j), EDGE, 
     &                              nedges, pedges)
            if (istat .NE. 0) then
              write(*,*) ' Body', i, ' Face', j,
     &                   ' Edge IG_getBodyTopos =', istat
              go to 2
            endif
            write(*,*) '   Face = ', j, '  nEdges = ', nedges
            large  = 0.0
            do 4 k = 1, nedges
c             get the parameter range for the Edge
              istat = IG_getInfo(edges(k), oclass, mtype, ref,
     &                           prev, next)
              if (istat .NE. 0) then
                write(*,*) ' Body', i, ' Edge', k,
     &                     ' IG_getInfo =', istat
                go to 4
              endif
	      if (mtype .EQ. DEGENERATE) go to 4
              istat = IG_getRange(edges(k), limits, iper)
	      if (istat .NE. 0) then
                write(*,*) ' Body', i, ' Face', j, ' Edge', k,
     &                     ' IG_getRange =', istat
	        go to 4
	      endif
              do 5 l = 1, 36
c               get the Face UV at t along the Edge interior
                t = limits(1) + l*(limits(2)-limits(1))/37.0
c               note: sense should be set for Edge in Face 2 times
	        istat = IG_getEdgeUV(faces(j), edges(k), 0, t, uv)
	        if (istat .NE. 0) then
                  write(*,*) ' Body', i, ' Face', j, ' Edge', k,
     &                       ' t = ', t, ' IG_getEdgeUV =', istat
	          go to 5
	        endif
c               get the Face XYZ for the UV
	        istat = IG_evaluate(faces(j), uv, result)
	        if (istat .NE. 0) then
                  write(*,*) ' Body', i, ' Face', j, ' Edge', k,
     &                       ' t = ', t, ' IG_evaluateF =', istat
	          go to 5
	        endif
c               get the Edge XYZ at the t parameter
	        istat = IG_evaluate(edges(k), t, eres)
	        if (istat .NE. 0) then
                  write(*,*) ' Body', i, ' Face', j, ' Edge', k,
     &                       ' t = ', t, ' IG_evaluateE =', istat
	          go to 5
	        endif
	        size = dsqrt((eres(1)-result(1))*(eres(1)-result(1)) +
     &                       (eres(2)-result(2))*(eres(2)-result(2)) +
     &                       (eres(3)-result(3))*(eres(3)-result(3)))
	        if (size .GT. large) large = size
 5            continue
 4          continue
            write(*,*) '     Edges large = ', large
            if (large .GT. sum) sum = large
            call IG_free(pedges)
 2        continue
	  call IG_free(pfaces)
c
c         look at total edge closure
c
	  istat = IG_getBodyTopos(bodies(i), null, EDGE, nedges, pedges)
	  if (istat .NE. 0) then
            write(*,*) ' Body', i, ' IG_getBodyTopos =', istat
	    go to 1
	  endif
          large  = 0.0
          do 6 k = 1, nedges
c           get the t range and the Nodes
            istat = IG_getTopology(edges(k), geom8, oclass, mtype, 
     &                             limits, nnodes, pnodes, psenses)
            if (istat .NE. 0) then
              write(*,*) ' Body', i, ' Edge', k,
     &                   ' IG_getTopologyE =', istat
              go to 6
            endif
            if (mtype .EQ. DEGENERATE) go to 3
            do 7 j = 1, nnodes
c             evaluate at the Edge bounds to get XYZ
              istat = IG_evaluate(edges(k), limits(j), eres)
              if (istat .NE. 0) then
                write(*,*) ' Body', i, ' Edge', k, j,
     &                     ' IG_evaluate =', istat
                go to 7
              endif
c	      get node position
              istat = IG_getTopology(nodes(j), geom8, oclass, mtype, 
     &                               xyz, ndum, pdum, psenses)
c             NOTE: nodes have no children
	      if (istat .NE. 0) then
                write(*,*) ' Body', i, ' Edge', k, j,
     &                     ' IG_getTopologyN =', istat
	        go to 7
	      endif
              size = dsqrt((eres(1)-xyz(1))*(eres(1)-xyz(1)) +
     &                     (eres(2)-xyz(2))*(eres(2)-xyz(2)) +
     &                     (eres(3)-xyz(3))*(eres(3)-xyz(3)))
              if (size .GT. large) large = size 
 7          continue
c           do not free in C/C++
 3          call IG_free(pnodes)
 6        continue
          call IG_free(pedges)
          write(*,*) ' '
          write(*,*) '   Faces/Edges large = ', sum
          write(*,*) '   Edges/Nodes large = ', large
c
 1	continue
c
c       do not free in C/C++
        call IG_free(pbody)
        write(*,*) ' '
c
        istat = IG_deleteObject(model8)
        write(*,*) 'IG_deleteObject = ', istat
c
	istat = IG_close(context8)
	write(*,*) 'IG_close = ', istat
c
	call exit
	end
