	program main
c
	include 'egads.inc'
	integer*8    context8, model8
	character*80 name

	istat = IG_open(context8)
	write(*,*) 'IG_open = ', istat

	write(*,*) 'Enter Filename:'
	read(*,'(a)') name

	istat = IG_loadModel(context8, 0, name, model8)
	write(*,*) 'IG_loadModel = ', istat
        
        write(*,*) ' '
        
        call parseOut(0, model8, 0)
        
        write(*,*) ' '
        
        istat = IG_deleteObject(model8)
        write(*,*) 'IG_deleteObject = ', istat

	istat = IG_close(context8)
	write(*,*) 'IG_close = ', istat
c
	call exit
	end


        subroutine parseOut(level, object, sense)
        include 'egads.inc'
c
c       recursive routine that outputs egos
c
        integer*8    object
        integer      level, sense
c
        pointer      (psenses, senses), (pivec, ivec)
        pointer      (prvec, rvec), (pobjs, objs)
        character*12 classType(27), curvType(9), surfType(11)
        integer*8    prev, next, geom, top, objs(*)
        integer      istat, oclass, mtype, nobj, per, senses(*), ivec(*)
        real*8       limits(4), rvec(*)
        data classType/'CONTEXT', 'TRANSFORM', 'TESSELLATION',
     &                 'NIL', 'EMPTY', 'REFERENCE', 4*' ', 
     &                 'PCURVE', 'CURVE', 'SURFACE', 7*' ', 'NODE',
     &                 'EGDE', 'LOOP', 'FACE', 'SHELL',
     &                 'BODY', 'MODEL'/
        data curvType/ 'Line', 'Circle', 'Ellipse', 'Parabola',
     &                 'Hyperbola', 'Trimmed', 'Bezier', 'BSpline', 
     &                 'Offset'/
        data surfType/ 'Plane', 'Spherical', 'Cylinder', 'Revolution',
     &                 'Toroidal', 'Trimmed', 'Bezier', 'BSpline', 
     &                 'Offset', 'Conical', 'Extrusion'/
        
c
c       get information on our object
        istat = IG_getInfo(object, oclass, mtype, top, prev, next)
        if (istat .NE. 0) then
          write(*,*) 'parseOut: ', level, ' IG_getInfo return = ', istat
          return
        endif
c
c       look at geometry
        if (oclass .GE. PCURVE .AND. oclass .LE. SURFACE) then
          istat = IG_getGeometry(object, oclass, mtype, geom, pivec,
     &                           prvec)
          if (istat .NE. 0) then
            write(*,*) 'parseOut: ', level, ' IG_getGeometry ret = ', 
     &                 istat
            return
          endif
          istat = IG_getRange(object, limits, per)
          if (istat .NE. 0) then
            write(*,*) 'parseOut: ', level, ' IG_getRange ret = ', 
     &                 istat
            return
          endif
          do i = 1, level
            write(*,'(a2,$)') '  '
          enddo
c
          if (oclass .EQ. PCURVE) then
c
            write(*,2000) classType(oclass+1), object, limits(1), 
     &                    limits(2), per
2000        format(a12, z12, '  range = ', 1p2e16.8, '  per = ', i2)
            do i = 1, level+1
              write(*,'(a2,$)') '  '
            enddo
c
c           output most data except the axes info
            if (mtype .EQ. CIRCLE) then
              write(*,2001) curvType(mtype), rvec(7)
2001          format(a12, '  radius =', 1pe16.8)
            elseif (mtype .EQ. ELLIPSE) then
              write(*,2002) curvType(mtype), rvec(7), rvec(8)
2002          format(a12, '  major = ', 1pe16.8, '  minor = ', 1pe16.8)
            elseif (mtype .EQ. PARABOLA) then
              write(*,2003) curvType(mtype), rvec(7)
2003          format(a12, '  focus =', 1pe16.8)
            elseif (mtype .EQ. HYPERBOLA) then
              write(*,2002) curvType(mtype), rvec(7), rvec(8)
            elseif (mtype .EQ. TRIMMED) then
              write(*,2004) curvType(mtype), rvec(1), rvec(2)
2004          format(a12, '  first = ', 1pe16.8, '  last = ', 1pe16.8)
            elseif (mtype .EQ. BEZIER) then
              write(*,2005) curvType(mtype), ivec(1), ivec(2), ivec(3)
2005          format(a12, '  flags = ', z3, '  degree = ', i3, '  #CP =',
     &               i4)
            elseif (mtype .EQ. BSPLINE) then
              write(*,2006) curvType(mtype), ivec(1), ivec(2), ivec(3), 
     &                      ivec(4)
2006          format(a12, '  flags = ', z3, '  degree = ', i3, 
     &               '  #CP =', i4, '  #knots = ', i4)
            elseif (mtype .EQ. OFFSET) then
              write(*,2007) curvType(mtype), rvec(1)
2007          format(a12, '  offset =', 1pe16.8)
            elseif (mtype .EQ. 0) then
              write(*,*) 'unknown curve type!'
            else 
              write(*,'(a12)') curvType(mtype)
            endif
c

          elseif (oclass .EQ. CURVE) then
c
            write(*,2000) classType(oclass+1), object, limits(1), 
     &                    limits(2), per
            do i = 1, level+1
              write(*,'(a2,$)') '  '
            enddo
c
c           output most data except the axes info
            if (mtype .EQ. CIRCLE) then
              write(*,2001) curvType(mtype), rvec(10)
            elseif (mtype .EQ. ELLIPSE) then
              write(*,2002) curvType(mtype), rvec(10), rvec(11)
            elseif (mtype .EQ. PARABOLA) then
              write(*,2003) curvType(mtype), rvec(10)
            elseif (mtype .EQ. HYPERBOLA) then
              write(*,2002) curvType(mtype), rvec(10), rvec(11)
            elseif (mtype .EQ. TRIMMED) then
              write(*,2004) curvType(mtype), rvec(1), rvec(2)
            elseif (mtype .EQ. BEZIER) then
              write(*,2005) curvType(mtype), ivec(1), ivec(2), ivec(3)
            elseif (mtype .EQ. BSPLINE) then
              write(*,2006) curvType(mtype), ivec(1), ivec(2), ivec(3), 
     &                      ivec(4)
            elseif (mtype .EQ. OFFSET) then
              write(*,2007) curvType(mtype), rvec(4)
            elseif (mtype .EQ. 0) then
              write(*,*) 'unknown curve type!'
            else 
              write(*,'(a12)') curvType(mtype)
            endif
c
          else
c
            write(*,3000) classType(oclass+1), object, limits(1), 
     &                    limits(2), limits(3), limits(4), per
3000        format(a12, z12, '  Urange = ', 1p2e16.8, 
     &             '  Urange = ', 1p2e16.8,'  per = ', i2)
            do i = 1, level+1
              write(*,'(a2,$)') '  '
            enddo
c
c           output most data except the axes info
            if (mtype .EQ. SPHERICAL) then
              write(*,2001) surfType(mtype), rvec(10)
            elseif (mtype .EQ. CONICAL) then
              write(*,3001) surfType(mtype), rvec(13), rvec(14)
3001          format(a12, '  angle = ', f12.6, '  radius = ', 1pe16.8)
            elseif (mtype .EQ. CYLINDER) then
              write(*,2001) surfType(mtype), rvec(13)
            elseif (mtype .EQ. TOROIDAL) then
              write(*,2002) surfType(mtype), rvec(13), rvec(14)
            elseif (mtype .EQ. BEZIER) then
              write(*,3002) surfType(mtype), ivec(1), ivec(2), ivec(3), 
     &                      ivec(4), ivec(5)
3002          format(a12, '  flags = ', z3, ' U deg = ', i3, ' #CP =',
     &               i4, ',  V deg = ',i3, ' #CP =', i4)
            elseif (mtype .EQ. BSPLINE) then
              write(*,3003) surfType(mtype), ivec(1), ivec(2), ivec(3), 
     &                      ivec(4), ivec(5), ivec(6), ivec(7)
3003          format(a12, '  flags = ', z3, ' U deg = ', i3, ' #CP =',
     &               i4, ' #knots = ', i4, ', V deg = ', i3, ' #CP =', 
     &               i4, ' #knots = ', i4)
            elseif (mtype .EQ. OFFSET) then
              write(*,2007) surfType(mtype), rvec(1)
            elseif (mtype .EQ. 0) then
              write(*,*) 'unknown surface type!'
            else 
              write(*,'(a12)') surfType(mtype)
            endif
c
          endif
c
          if (pivec .NE. 0) call IG_free(pivec)
          if (prvec .NE. 0) call IG_free(prvec)
          if (geom  .NE. 0) call parseOut(level+1, geom, 0)
          return
        endif
c
c       output the object class and sense
        do i = 1, level
          write(*,'(a2,$)') '  '
        enddo
        if (sense .eq. 0) then
          write(*, 1001) classType(oclass+1), object
1001      format(a12, z12)
        else
          write(*,1002) classType(oclass+1), object, sense
1002      format(a12, z12, '  sense = ', i2)
        endif
c
c       do the topology
        if (oclass .GE. NODE .AND. oclass .LE. MODEL) then
          istat = IG_getTopology(object, geom, oclass, mtype, limits, 
     &                           nobj, pobjs, psenses)
          if (istat .NE. 0) then
            write(*,*) 'parseOut: ', level, ' IG_getTopology ret = ', 
     &                 istat
            return
          endif
c         extract the data from the limits array
          if (oclass .EQ. NODE) then
            do i = 1, level+1
              write(*,'(a2,$)') '  '
            enddo
            write(*,1003) limits(1), limits(2), limits(3)
1003        format('XYZ = ', 1p3e16.8)
          elseif (oclass .EQ. EDGE) then
            do i = 1, level+1
              write(*,'(a2,$)') '  '
            enddo
	    if (mtype .EQ. DEGENERATE) then
              write(*,1006) limits(1), limits(2)
1006          format('tRange = ', 1p2e16.8,' -- Degenerate!')
	    else
              write(*,1004) limits(1), limits(2)
1004          format('tRange = ', 1p2e16.8)
	    endif
          elseif (oclass .EQ. FACE) then
            do i = 1, level+1
              write(*,'(a2,$)') '  '
            enddo
            write(*,1005) limits(1), limits(2), limits(3), limits(4)
1005        format('uRange = ', 1p2e16.8, '  vRange = ', 1p2e16.8)
          endif
c         recurse on children & geometry
          if (geom .NE. 0 .AND. mtype .NE. DEGENERATE)
     &      call parseOut(level+1, geom, 0)
          do i = 1, nobj
            if (psenses .EQ. 0) then
              call parseOut(level+1, objs(i), 0)
            else
              call parseOut(level+1, objs(i), senses(i))
            endif
          enddo
          if (geom .NE. 0 .AND. oclass .EQ. LOOP) then
            do i = 1, nobj
              call parseOut(level+1, objs(i+nobj), 0)
            enddo
          endif
c         the following is NOT needed in C/C++
          if (nobj .ne. 0) call IG_free(pobjs)
        endif
c  
        return
        end
