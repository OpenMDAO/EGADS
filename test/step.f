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

	istat = IG_saveModel(model8, 'egads.step')
	write(*,*) 'IG_saveModel     = ', istat

	istat = IG_deleteObject(model8)
	write(*,*) 'IG_deleteObject = ', istat

	istat = IG_close(context8)
	write(*,*) 'IG_close = ', istat
c
	call exit
	end
