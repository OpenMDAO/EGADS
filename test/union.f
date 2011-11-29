	program main
c
	include 'egads.inc'
c
	pointer      (psenses, senses), (pbody, bodies)
        pointer      (pbodyt, bodiest)
        integer      oclass, senses(*)
	integer*8    null, context8, model8, tool8, newmodel8, bod(2)
	integer*8    bodies(*), bodiest(*), geom8
        real*8       limits(4)
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
        if (istat .NE. 0) call exit(1)
c
c       get the bodies
c
 1      istat = IG_getTopology(model8, geom8, oclass, mtype, limits, 
     &                         nbodies, pbody, psenses)
	if (istat .NE. 0) then
	  write(*,*) 'IG_getTopology =', istat
	  go to 2
	endif
	write(*,*) 'nBodies = ', nbodies
        if (nBodies .NE. 1) go to 2
c
c
  	write(*,*) 'Enter Filename:'
	read(*,'(a)') name
        if (name .EQ. ' ') then
c         do not free in C/C++
          call IG_free(pbody)
          go to 2
        endif
c
c       load the file
c
	istat = IG_loadModel(context8, 0, name, tool8)
        if (istat .NE. 0) then
	  write(*,*) 'IG_loadModel = ', istat
          write(*,*) ' '
c         do not free in C/C++
          call IG_free(pbody)
          go to 2          
        endif
c
c       get the bodies
c
        istat = IG_getTopology(tool8, geom8, oclass, mtype, limits, 
     &                         nbodiest, pbodyt, psenses)
	if (istat .NE. 0) then
	  write(*,*) 'IG_getTopology =', istat
c         do not free in C/C++
          call IG_free(pbody)
	  go to 2
	endif
	write(*,*) 'nBodies = ', nbodiest
        if (nBodies .NE. 1) then
c         do not free in C/C++
          call IG_free(pbody)
          go to 2
        endif
c
        istat = IG_solidBoolean(bodies(1), bodiest(1), 3, newmodel8)
        if (istat .NE. 0) then
          write(*,*) 'IG_solidBoolean =', istat
          call IG_copyObject(bodies(1),  null, bod(1))
          call IG_copyObject(bodiest(1), null, bod(2))
          istat  = IG_makeTopology(context8, 0, MODEL, SOLIDBODY,
     &                             limits, 2, bod, 0, newmodel8)
          istat  = IG_deleteObject(tool8)
          istat  = IG_deleteObject(model8)
          model8 = newmodel8          
c         do not free in C/C++
          call IG_free(pbody)
          call IG_free(pbodyt)
          go to 2
        endif
        istat = IG_deleteObject(tool8)
        write(*,*) 'IG_deleteObject = ', istat
        istat = IG_deleteObject(model8)
        write(*,*) 'IG_deleteObject = ', istat
        
        model8 = newmodel8
        go to 1
c
c
 2      istat = IG_saveModel(model8, 'union.egads')
        write(*,*) 'IG_saveModel = ', istat
c       istat = IG_saveModel(model8, 'union.step')
c       write(*,*) 'IG_saveModel = ', istat
c
        istat = IG_deleteObject(model8)
        write(*,*) 'IG_deleteObject = ', istat
	istat = IG_close(context8)
	write(*,*) 'IG_close = ', istat
c
	call exit
	end
