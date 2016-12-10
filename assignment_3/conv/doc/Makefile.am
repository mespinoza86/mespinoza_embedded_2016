directory = $(top_srcdir)/docs/html/

dist_man_MANS = $(directory)/
$(directory)/: Doxygen.stamp

Doxygen.stamp:
	doxygen Doxygen
	echo Timestamp > Doxygen.stamp

CLEANFILES = Doxygen.stamp

all-local: Doxygen.stamp
clean-local:
	rm -rf $(top_srcdir)/docs/html/
