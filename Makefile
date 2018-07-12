

all:
	ndk-build NDK_APPLICATION_MK=./jni/Application.mk
clean:
	ndk-build clean
	rm -rf libs obj
