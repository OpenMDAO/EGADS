	program main
c
	include 'egads.inc'
	pointer (pbody, bodies), (pedge, edges), (ploop, loops)
	integer*8       bodies(*),       edges(*),       loops(*)
        pointer (psenses, senses)
	integer           senses(*)
	integer*8    context8, model8, geom8, topo8, null
	real*8       limits(4)
	character*80 name
	data null/0/

	istat = IG_open(context8)
	write(*,*) 'IG_open = ', istat

	write(*,*) 'Enter Filename:'
	read(*,'(a)') name

	istat = IG_loadModel(context8, 0, name, model8)
	write(*,*) 'IG_loadModel = ', istat

	istat = IG_getTopology(model8, geom8, iclass, mtype, limits,
     &                         nbody, pbody, psenses)
	write(*,*) 'IG_getTopology = ', istat, nbody

	istat = IG_getBodyTopos(bodies(1), null, LOOP, nloop, ploop)
	write(*,*) 'IG_getBodyTopos = ', istat, nloop

	do i = 1, nloop
	  istat = IG_getTopology(loops(i), geom8, iclass, mtype, limits,
     &                           nedges, pedge, psenses)
	  write(*,*) i, ' IG_getTopology  = ', istat, nedges

	  istat = IG_makeTopology(context8, null, LOOP, 0, limits,
     &                            nedges, edges, senses, topo8)
	  write(*,*) i, ' IG_makeTopology = ', istat

          istat = IG_deleteObject(topo8)
	  call IG_free(pedge)
	enddo

	call IG_free(ploop)
	call IG_free(pbody)

	istat = IG_deleteObject(model8)
	write(*,*) 'IG_deleteObject = ', istat

	istat = IG_close(context8)
	write(*,*) 'IG_close = ', istat
c
	call exit
	end
