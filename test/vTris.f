	program main
c
	include 'egads.inc'
	include 'gv.inc'
c
	pointer      (psenses, senses), (pbody,  bodies), (ptess, tess)
	pointer      (pfaces,  faces),  (pgo,    gohs)
	pointer      (ptypes,  type),   (pindex, indices)
	pointer      (ptris,   tris),   (ptric,  tric)
	pointer      (pxyzs,   xyzs),   (puvs,   uvs)
c
	integer      oclass, mtype, keys(2), types(2), senses(*)
	integer      gohs(*), type(*), indices(*), tris(*), tric(*)
	integer*8    null, context8, model8, geom8
	integer*8    faces(*), bodies(*), tess(*)
	real*8       boxx(6), size, focus(4), lims(4), parms(3), rlims(4)
	real*8       xyzs(*), uvs(*)
	real         fcolor(3), bcolor(3)
	character*16 titles(2), gname
	character*80 name
c
	common/vTris/ ng, nbody, pgo, pbody, ptess
c
	data null/0/
	data keys/117, 118/
	data types/GV_SURF, GV_SURF/
	data lims/0.0, 2.0, 0.0, 2.0/
	data bcolor/0.5, 0.5, 0.5/
	data titles/'U Parameter', 'V Parameter'/

	istat = IG_open(context8)
	write(*,*) 'IG_open = ', istat

	write(*,*) 'Enter Filename:'
	read(*,'(a)') name

	istat = IG_loadModel(context8, 0, name, model8)
	if(istat .NE. 0) then
	  write(*,*) 'IG_loadModel =', istat
	  call exit(1)
	endif

        write(*,*) ' '
       
	istat = IG_getBoundingBox(model8, boxx)
	size  = sqrt((boxx(1)-boxx(4))*(boxx(1)-boxx(4)) + 
     &               (boxx(2)-boxx(5))*(boxx(2)-boxx(5)) +
     &               (boxx(3)-boxx(6))*(boxx(3)-boxx(6)))
	focus(1) = 0.5*(boxx(1)+boxx(4))
	focus(2) = 0.5*(boxx(2)+boxx(5))
	focus(3) = 0.5*(boxx(3)+boxx(6))
	focus(4) = size
	parms(1) =  0.025*size
	parms(2) =  0.001*size
	parms(3) = 12.0

C	get all bodies 
	istat = IG_getTopology(model8, geom8, oclass, mtype, rlims,
     &                         nbody, pbody, psenses)
	if(istat .NE. 0) then
	  write(*,*) 'IG_getTopology =', istat
	  call exit(1)
	endif
	write(*,*) 'IG_getTopology: nBodies =', nbody
	istat = IG_alloc(nbody*8, ptess)
	if(istat .NE. 0) then
	  write(*,*) 'IG_alloc tess =', istat
	  call exit(1)
	endif

C       count the number of graphics (static in this case)
	ng = 0
	do 1 i = 1, nbody
	  tess(i) = null
	  istat = IG_getBodyTopos(bodies(i), null, FACE, nfaces, pfaces)
	  if(istat .NE. 0) then
	    write(*,*) ' Body', i, ' IG_getBodyTopos =', istat
	    go to 1
	  endif
	  if(nfaces .EQ. 0) go to 1
	  istat = IG_makeTessBody(bodies(i), parms, tess(i))
	  if(stat .NE. 0) then
	    write(*,*) 'IG_makeTessBody', i, ' =', istat
	    go to 1 
          endif
	  ng = ng + nfaces
 1	continue
	if(ng .NE. 0) then

C         we have things to plot, get storage for the graphic objects
	  istat = IG_alloc(4*ng, pgo)
	  if(istat .NE. 0) then
	    write(*,*) 'IG_alloc graphics =', istat
	    call exit(1)
	  endif

C         make the graphic objects and save the handles
          n = 0
	  do 2 i = 1, nbody
	    fcolor(1) = 1.0
	    fcolor(2) = float(i-1)/(nbody-1)
	    fcolor(3) = 0.0
	    istat = IG_getBodyTopos(bodies(i), null, FACE, nfaces,
     &                              pfaces)
	    if(istat .NE. 0) then
	      write(*,*) ' Body', i, ' IG_getBodyTopos =', istat
	      go to 2
	    endif
	    do 3 j = 1, nfaces
              n       = n + 1
	      gohs(n) = 0
	      istat = IG_getTessFace(tess(i), j, len, pxyzs, puvs,
     &                               ptypes, pindex, ntri, ptris, ptric)
	      if(istat .NE. 0)
     &          write(*,*) ' tessFace', i, j, ' =', istat
	      if(istat .NE. 0 .or. ntri .EQ. 0) go to 3
	      write(gname,1001) i, j
 1001	      format('Body ',i2,' Face ',i3)
	      gohs(n) = FGV_create(GV_DISJOINTTRIANGLES,
     &                             GV_FOREGROUND+GV_ORIENTATION,
     &                             fcolor, bcolor, gname,
     &                             ntri, 0, tris, xyzs, i, j)
              boxx(1) = uvs(1)
              boxx(2) = uvs(2)
              boxx(3) = uvs(1)
              boxx(4) = uvs(2)
              do k = 2, len
                if(uvs(2*k-1) .LT. boxx(1)) boxx(1) = uvs(2*k-1)
                if(uvs(2*k-1) .GT. boxx(3)) boxx(3) = uvs(2*k-1)
                if(uvs(2*k)   .LT. boxx(2)) boxx(2) = uvs(2*k)
                if(uvs(2*k)   .GT. boxx(4)) boxx(4) = uvs(2*k)
              enddo
              write(*,1002) gname, boxx(1),boxx(3), boxx(2),boxx(4), 
     &                      gohs(n)
 1002         format(1x, a, '  Urange =',2f9.3, '  Vrange =',2f9.3, i4)
 3          continue
 2	  continue

C         plot the faces
	  istat = FGV_init('     vTris test code', 0, 2, keys, types,
     &                     lims, titles, focus)
          write(*,*) 'FGV_init =', istat
	  call FGV_cleanup()
	  call IG_free(pgo)

	else

          write(*,*) ' ERROR: No tessellations!'

	endif

C       cleanup & close
	do i = 1, nbody
	  if(tess(i) .NE. null) istat = IG_deleteObject(tess(i))
        enddo
	call IG_free(ptess)
        istat = IG_deleteObject(model8)
        write(*,*) 'IG_deleteObject = ', istat
	istat = IG_close(context8)
	write(*,*) 'IG_close = ', istat
c
	call exit
	end


C       required call-backs


	integer function FGVupdate()
c
	pointer   (pbody, bodies), (ptess, tess), (pgo, gohs)
	integer   gohs(*)
	integer*8 bodies(*), tess(*)
	common/vTris/ ng, nbody, pgo, pbody, ptess
c
	FGVupdate = ng
	return
	end


	subroutine FGVdata(ngrap, graphand)
c
	integer ngrap, graphand(*)
c
	pointer   (pbody, bodies), (ptess, tess), (pgo, gohs)
	integer   gohs(*)
	integer*8 bodies(*), tess(*)
	common/vTris/ ng, nbody, pgo, pbody, ptess
c
	do i = 1, ng
	  graphand(i) = gohs(i)
	enddo
c
	return
	end


	integer function FGVscalar(key, gohi, len, scalar)
c
	integer key, gohi, len
	real    scalar(*)
c
	include 'egads.inc'
c
	pointer   (pbody, bodies), (ptess, tess), (pgo, gohs) 
	pointer   (ptypes, type), (pindex, indices)
	pointer   (ptris, tris), (ptric, tric)
	pointer   (pxyzs, xyzs), (puvs, uvs), (pfaces, faces)
	integer   gohs(*), type(*), indices(*), tris(*), tric(*)
	integer*8 bodies(*), tess(*), faces(*), null
	real*8    xyzs(*), uvs(*)
	common/vTris/ ng, nbody, pgo, pbody, ptess
c
        data null/0/
c
        n = 0
        do 1 i = 1, nbody
          istat = IG_getBodyTopos(bodies(i), null, FACE, nfaces,
     &                            pfaces)
          if(istat .NE. 0) go to 1
          do 2 j = 1, nfaces
            n = n + 1
            istat = IG_getTessFace(tess(i), j, length, pxyzs, puvs,
     &                             ptypes, pindex, ntri, ptris, ptric)
            if(istat .NE. 0 .or. ntri .EQ. 0) go to 2
            if(gohs(n) .NE. gohi) go to 2
            if(key .EQ. 1) then
              do k = 1, len
                scalar(k) = uvs(2*k-1)
              enddo
            else
              do k = 1, len
                scalar(k) = uvs(2*k)
              enddo
            endif
            FGVscalar = 1
            return
 2        continue
 1	continue

	FGVscalar = 0
	return
	end
