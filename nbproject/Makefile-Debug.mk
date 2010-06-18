#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=
AS=as

# Macros
CND_PLATFORM=GNU-MacOSX
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/BlammoEngine/Light.o \
	${OBJECTDIR}/ESPEngine/ESPParticleRotateEffector.o \
	${OBJECTDIR}/BlammoEngine/FBObj.o \
	${OBJECTDIR}/include/zlib/adler32.o \
	${OBJECTDIR}/include/zlib/trees.o \
	${OBJECTDIR}/GameView/GameItemAssets.o \
	${OBJECTDIR}/GameModel/NormalBallState.o \
	${OBJECTDIR}/include/zlib/infback.o \
	${OBJECTDIR}/ESPEngine/ESPParticleAccelEffector.o \
	${OBJECTDIR}/GameModel/BallWormholeState.o \
	${OBJECTDIR}/BlammoEngine/Mesh.o \
	${OBJECTDIR}/GameView/GameWorldAssets.o \
	${OBJECTDIR}/GameModel/SolidBlock.o \
	${OBJECTDIR}/GameModel/BallSizeItem.o \
	${OBJECTDIR}/include/zlib/crc32.o \
	${OBJECTDIR}/GameView/CgFxPrism.o \
	${OBJECTDIR}/GameModel/PaddleCamItem.o \
	${OBJECTDIR}/GameModel/BoundingLines.o \
	${OBJECTDIR}/BlammoEngine/Texture2D.o \
	${OBJECTDIR}/GameView/CgFxStickyPaddle.o \
	${OBJECTDIR}/GameModel/GameWorld.o \
	${OBJECTDIR}/ESPEngine/ESPEmitter.o \
	${OBJECTDIR}/GameModel/CollateralBlockProjectile.o \
	${OBJECTDIR}/GameModel/MultiBallItem.o \
	${OBJECTDIR}/ESPEngine/ESPVolumeEmitter.o \
	${OBJECTDIR}/include/physfs/dir.o \
	${OBJECTDIR}/include/zlib/compress.o \
	${OBJECTDIR}/GameView/GameCompleteDisplayState.o \
	${OBJECTDIR}/BlammoEngine/Skybox.o \
	${OBJECTDIR}/GameView/FlashHUD.o \
	${OBJECTDIR}/GameModel/GameOverState.o \
	${OBJECTDIR}/include/physfs/windows.o \
	${OBJECTDIR}/BlammoEngine/TextLabel.o \
	${OBJECTDIR}/GameModel/LaserBeamPaddleItem.o \
	${OBJECTDIR}/GameModel/GameLevel.o \
	${OBJECTDIR}/include/physfs/posix.o \
	${OBJECTDIR}/GameView/InGameMenuState.o \
	${OBJECTDIR}/ESPEngine/ESPParticle.o \
	${OBJECTDIR}/GameView/CgFxPostUberIntense.o \
	${OBJECTDIR}/ESPEngine/ESPParticleScaleEffector.o \
	${OBJECTDIR}/include/physfs/zip.o \
	${OBJECTDIR}/GameSound/Sound.o \
	${OBJECTDIR}/GameModel/PaddleLaser.o \
	${OBJECTDIR}/BlammoEngine/ObjReader.o \
	${OBJECTDIR}/GameView/LaserPaddleGun.o \
	${OBJECTDIR}/GameModel/BallDeathState.o \
	${OBJECTDIR}/GameModel/PrismBlock.o \
	${OBJECTDIR}/ResourceManager.o \
	${OBJECTDIR}/GameModel/UpsideDownItem.o \
	${OBJECTDIR}/ESPEngine/ESPMultiColourEffector.o \
	${OBJECTDIR}/GameView/CgFxInkBlock.o \
	${OBJECTDIR}/GameModel/BallCamItem.o \
	${OBJECTDIR}/include/zlib/inflate.o \
	${OBJECTDIR}/include/zlib/inffast.o \
	${OBJECTDIR}/ESPEngine/ESPUtil.o \
	${OBJECTDIR}/GameView/PrismBlockMesh.o \
	${OBJECTDIR}/GameSound/GameSoundAssets.o \
	${OBJECTDIR}/GameModel/BallInPlayState.o \
	${OBJECTDIR}/GameModel/LevelPiece.o \
	${OBJECTDIR}/GameModel/PaddleRocketProjectile.o \
	${OBJECTDIR}/BlammoEngine/Camera.o \
	${OBJECTDIR}/GameModel/GameTransformMgr.o \
	${OBJECTDIR}/BlammoEngine/Point.o \
	${OBJECTDIR}/GameModel/StickyPaddleItem.o \
	${OBJECTDIR}/GameModel/BallSafetyNetItem.o \
	${OBJECTDIR}/GameModel/OneUpItem.o \
	${OBJECTDIR}/BlammoEngine/mtrand.o \
	${OBJECTDIR}/GameView/LevelMesh.o \
	${OBJECTDIR}/GameModel/CannonBlock.o \
	${OBJECTDIR}/ESPEngine/ESPOnomataParticle.o \
	${OBJECTDIR}/GameModel/CollateralBlock.o \
	${OBJECTDIR}/GameView/MainMenuDisplayState.o \
	${OBJECTDIR}/GameController.o \
	${OBJECTDIR}/GameView/CgFxGaussianBlur.o \
	${OBJECTDIR}/BlammoEngine/Texture3D.o \
	${OBJECTDIR}/GameView/GameDisplay.o \
	${OBJECTDIR}/BlammoEngine/Texture1D.o \
	${OBJECTDIR}/include/physfs/beos.o \
	${OBJECTDIR}/GameModel/GameItemFactory.o \
	${OBJECTDIR}/GameModel/BombBlock.o \
	${OBJECTDIR}/include/zlib/uncompr.o \
	${OBJECTDIR}/GameModel/UberBallItem.o \
	${OBJECTDIR}/BlammoEngine/Algebra.o \
	${OBJECTDIR}/GameView/LoadingScreen.o \
	${OBJECTDIR}/GameView/LivesLeftHUD.o \
	${OBJECTDIR}/GameModel/PoisonPaddleItem.o \
	${OBJECTDIR}/GameModel/InkBlock.o \
	${OBJECTDIR}/GameModel/BallOnPaddleState.o \
	${OBJECTDIR}/GameView/CgFxFullscreenGoo.o \
	${OBJECTDIR}/GameModel/PaddleSizeItem.o \
	${OBJECTDIR}/BlammoEngine/MtlReader.o \
	${OBJECTDIR}/GameView/GameAssets.o \
	${OBJECTDIR}/GameModel/WorldCompleteState.o \
	${OBJECTDIR}/GameModel/GameCompleteState.o \
	${OBJECTDIR}/GameView/GameMenu.o \
	${OBJECTDIR}/GameSound/GameSound.o \
	${OBJECTDIR}/GameView/GameLightAssets.o \
	${OBJECTDIR}/GameView/CgFxDecoSkybox.o \
	${OBJECTDIR}/include/physfs/os2.o \
	${OBJECTDIR}/ESPEngine/ESPParticleColourEffector.o \
	${OBJECTDIR}/GameModel/InvisiBallItem.o \
	${OBJECTDIR}/GameView/CrosshairLaserHUD.o \
	${OBJECTDIR}/include/physfs/pocketpc.o \
	${OBJECTDIR}/GameModel/GameEventManager.o \
	${OBJECTDIR}/include/zlib/deflate.o \
	${OBJECTDIR}/GameModel/RocketPaddleItem.o \
	${OBJECTDIR}/GameModel/InCannonBallState.o \
	${OBJECTDIR}/GameView/PlayerHurtHUD.o \
	${OBJECTDIR}/GameView/DecoSkybox.o \
	${OBJECTDIR}/GameView/CgFxPortalBlock.o \
	${OBJECTDIR}/GameView/GameEventsListener.o \
	${OBJECTDIR}/GameModel/LaserPaddleItem.o \
	${OBJECTDIR}/GameView/CgFxPostSmokey.o \
	${OBJECTDIR}/GameView/CgFxPostRefract.o \
	${OBJECTDIR}/GameModel/GameState.o \
	${OBJECTDIR}/BlammoEngine/Noise.o \
	${OBJECTDIR}/GameModel/GameBall.o \
	${OBJECTDIR}/GameView/PaddleRocketMesh.o \
	${OBJECTDIR}/GameModel/GameItemTimer.o \
	${OBJECTDIR}/GameModel/GameModel.o \
	${OBJECTDIR}/GameView/StickyPaddleGoo.o \
	${OBJECTDIR}/GameView/BallSafetyNetMesh.o \
	${OBJECTDIR}/GameView/GameFontAssetsManager.o \
	${OBJECTDIR}/ESPEngine/ESPParticleBatchMesh.o \
	${OBJECTDIR}/GameModel/Beam.o \
	${OBJECTDIR}/BlammoEngine/TextureCube.o \
	${OBJECTDIR}/GameView/GameMenuItem.o \
	${OBJECTDIR}/include/physfs/physfs.o \
	${OBJECTDIR}/GameModel/GameItem.o \
	${OBJECTDIR}/GameModel/BlackoutItem.o \
	${OBJECTDIR}/GameView/CgFxBloom.o \
	${OBJECTDIR}/GameModel/LevelCompleteState.o \
	${OBJECTDIR}/BlammoEngine/CgFxEffect.o \
	${OBJECTDIR}/GameModel/GhostBallItem.o \
	${OBJECTDIR}/ESPEngine/ESPShaderParticle.o \
	${OBJECTDIR}/include/zlib/gzio.o \
	${OBJECTDIR}/include/physfs/physfs_byteorder.o \
	${OBJECTDIR}/GameView/GameESPAssets.o \
	${OBJECTDIR}/include/zlib/zutil.o \
	${OBJECTDIR}/BlammoEngine/TextureFontSet.o \
	${OBJECTDIR}/GameView/CgFxPhong.o \
	${OBJECTDIR}/BlammoEngine/GeometryMaker.o \
	${OBJECTDIR}/GameView/GameViewConstants.o \
	${OBJECTDIR}/GameView/GameOverDisplayState.o \
	${OBJECTDIR}/GameView/CollateralBlockMesh.o \
	${OBJECTDIR}/GameModel/PlayerPaddle.o \
	${OBJECTDIR}/include/physfs/physfs_unicode.o \
	${OBJECTDIR}/BlammoEngine/Texture.o \
	${OBJECTDIR}/GameModel/TriangleBlocks.o \
	${OBJECTDIR}/GameView/StartGameDisplayState.o \
	${OBJECTDIR}/BlammoEngine/BlammoTime.o \
	${OBJECTDIR}/GameSound/MSFReader.o \
	${OBJECTDIR}/GameView/PortalBlockMesh.o \
	${OBJECTDIR}/GameView/CgFxInkSplatter.o \
	${OBJECTDIR}/GameView/GameFBOAssets.o \
	${OBJECTDIR}/GameModel/PortalBlock.o \
	${OBJECTDIR}/ESPEngine/ESPPointEmitter.o \
	${OBJECTDIR}/WindowManager.o \
	${OBJECTDIR}/GameView/CgFxCelShading.o \
	${OBJECTDIR}/GameModel/BallSpeedItem.o \
	${OBJECTDIR}/GameMain.o \
	${OBJECTDIR}/include/physfs/unix.o \
	${OBJECTDIR}/GameModel/Projectile.o \
	${OBJECTDIR}/GameView/CgFxAfterImage.o \
	${OBJECTDIR}/GameView/CannonBlockMesh.o \
	${OBJECTDIR}/GameModel/GravityBallItem.o \
	${OBJECTDIR}/include/physfs/macosx.o \
	${OBJECTDIR}/ConfigOptions.o \
	${OBJECTDIR}/GameView/DecoWorldAssets.o \
	${OBJECTDIR}/include/SDLMain.o \
	${OBJECTDIR}/ESPEngine/ESPLineEmitter.o \
	${OBJECTDIR}/GameView/CgFxVolumetricEffect.o \
	${OBJECTDIR}/GameModel/GameModelConstants.o \
	${OBJECTDIR}/GameView/InGameDisplayState.o \
	${OBJECTDIR}/BlammoEngine/VBOBatch.o \
	${OBJECTDIR}/GameModel/Onomatoplex.o \
	${OBJECTDIR}/GameModel/BreakableBlock.o \
	${OBJECTDIR}/include/zlib/inftrees.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m32
CXXFLAGS=-m32

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-Llib -L/opt/local/lib -L/usr/local/lib -lphysfs -lfreetype -lalut lib/libjpeg.a lib/libpng14.a lib/libILU.a lib/libILUT.a lib/libIL.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Debug.mk dist/Debug/GNU-MacOSX/biffbamblammo

dist/Debug/GNU-MacOSX/biffbamblammo: lib/libjpeg.a

dist/Debug/GNU-MacOSX/biffbamblammo: lib/libpng14.a

dist/Debug/GNU-MacOSX/biffbamblammo: lib/libILU.a

dist/Debug/GNU-MacOSX/biffbamblammo: lib/libILUT.a

dist/Debug/GNU-MacOSX/biffbamblammo: lib/libIL.a

dist/Debug/GNU-MacOSX/biffbamblammo: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-MacOSX
	${LINK.cc} -framework GLUT -framework OpenGL -framework SDL -framework Cg -framework OpenAL -framework Carbon -framework IOKit -framework GLEW -framework Cocoa -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/biffbamblammo ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/BlammoEngine/Light.o: nbproject/Makefile-${CND_CONF}.mk BlammoEngine/Light.cpp 
	${MKDIR} -p ${OBJECTDIR}/BlammoEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/BlammoEngine/Light.o BlammoEngine/Light.cpp

${OBJECTDIR}/ESPEngine/ESPParticleRotateEffector.o: nbproject/Makefile-${CND_CONF}.mk ESPEngine/ESPParticleRotateEffector.cpp 
	${MKDIR} -p ${OBJECTDIR}/ESPEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/ESPEngine/ESPParticleRotateEffector.o ESPEngine/ESPParticleRotateEffector.cpp

${OBJECTDIR}/BlammoEngine/FBObj.o: nbproject/Makefile-${CND_CONF}.mk BlammoEngine/FBObj.cpp 
	${MKDIR} -p ${OBJECTDIR}/BlammoEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/BlammoEngine/FBObj.o BlammoEngine/FBObj.cpp

${OBJECTDIR}/include/zlib/adler32.o: nbproject/Makefile-${CND_CONF}.mk include/zlib/adler32.c 
	${MKDIR} -p ${OBJECTDIR}/include/zlib
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/zlib/adler32.o include/zlib/adler32.c

${OBJECTDIR}/include/zlib/trees.o: nbproject/Makefile-${CND_CONF}.mk include/zlib/trees.c 
	${MKDIR} -p ${OBJECTDIR}/include/zlib
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/zlib/trees.o include/zlib/trees.c

${OBJECTDIR}/GameView/GameItemAssets.o: nbproject/Makefile-${CND_CONF}.mk GameView/GameItemAssets.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/GameItemAssets.o GameView/GameItemAssets.cpp

${OBJECTDIR}/GameModel/NormalBallState.o: nbproject/Makefile-${CND_CONF}.mk GameModel/NormalBallState.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/NormalBallState.o GameModel/NormalBallState.cpp

${OBJECTDIR}/include/zlib/infback.o: nbproject/Makefile-${CND_CONF}.mk include/zlib/infback.c 
	${MKDIR} -p ${OBJECTDIR}/include/zlib
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/zlib/infback.o include/zlib/infback.c

${OBJECTDIR}/ESPEngine/ESPParticleAccelEffector.o: nbproject/Makefile-${CND_CONF}.mk ESPEngine/ESPParticleAccelEffector.cpp 
	${MKDIR} -p ${OBJECTDIR}/ESPEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/ESPEngine/ESPParticleAccelEffector.o ESPEngine/ESPParticleAccelEffector.cpp

${OBJECTDIR}/GameModel/BallWormholeState.o: nbproject/Makefile-${CND_CONF}.mk GameModel/BallWormholeState.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/BallWormholeState.o GameModel/BallWormholeState.cpp

${OBJECTDIR}/BlammoEngine/Mesh.o: nbproject/Makefile-${CND_CONF}.mk BlammoEngine/Mesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/BlammoEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/BlammoEngine/Mesh.o BlammoEngine/Mesh.cpp

${OBJECTDIR}/GameView/GameWorldAssets.o: nbproject/Makefile-${CND_CONF}.mk GameView/GameWorldAssets.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/GameWorldAssets.o GameView/GameWorldAssets.cpp

${OBJECTDIR}/GameModel/SolidBlock.o: nbproject/Makefile-${CND_CONF}.mk GameModel/SolidBlock.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/SolidBlock.o GameModel/SolidBlock.cpp

${OBJECTDIR}/GameModel/BallSizeItem.o: nbproject/Makefile-${CND_CONF}.mk GameModel/BallSizeItem.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/BallSizeItem.o GameModel/BallSizeItem.cpp

${OBJECTDIR}/include/zlib/crc32.o: nbproject/Makefile-${CND_CONF}.mk include/zlib/crc32.c 
	${MKDIR} -p ${OBJECTDIR}/include/zlib
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/zlib/crc32.o include/zlib/crc32.c

${OBJECTDIR}/GameView/CgFxPrism.o: nbproject/Makefile-${CND_CONF}.mk GameView/CgFxPrism.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/CgFxPrism.o GameView/CgFxPrism.cpp

${OBJECTDIR}/GameModel/PaddleCamItem.o: nbproject/Makefile-${CND_CONF}.mk GameModel/PaddleCamItem.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/PaddleCamItem.o GameModel/PaddleCamItem.cpp

${OBJECTDIR}/GameModel/BoundingLines.o: nbproject/Makefile-${CND_CONF}.mk GameModel/BoundingLines.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/BoundingLines.o GameModel/BoundingLines.cpp

${OBJECTDIR}/BlammoEngine/Texture2D.o: nbproject/Makefile-${CND_CONF}.mk BlammoEngine/Texture2D.cpp 
	${MKDIR} -p ${OBJECTDIR}/BlammoEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/BlammoEngine/Texture2D.o BlammoEngine/Texture2D.cpp

${OBJECTDIR}/GameView/CgFxStickyPaddle.o: nbproject/Makefile-${CND_CONF}.mk GameView/CgFxStickyPaddle.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/CgFxStickyPaddle.o GameView/CgFxStickyPaddle.cpp

${OBJECTDIR}/GameModel/GameWorld.o: nbproject/Makefile-${CND_CONF}.mk GameModel/GameWorld.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/GameWorld.o GameModel/GameWorld.cpp

${OBJECTDIR}/ESPEngine/ESPEmitter.o: nbproject/Makefile-${CND_CONF}.mk ESPEngine/ESPEmitter.cpp 
	${MKDIR} -p ${OBJECTDIR}/ESPEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/ESPEngine/ESPEmitter.o ESPEngine/ESPEmitter.cpp

${OBJECTDIR}/GameModel/CollateralBlockProjectile.o: nbproject/Makefile-${CND_CONF}.mk GameModel/CollateralBlockProjectile.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/CollateralBlockProjectile.o GameModel/CollateralBlockProjectile.cpp

${OBJECTDIR}/GameModel/MultiBallItem.o: nbproject/Makefile-${CND_CONF}.mk GameModel/MultiBallItem.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/MultiBallItem.o GameModel/MultiBallItem.cpp

${OBJECTDIR}/ESPEngine/ESPVolumeEmitter.o: nbproject/Makefile-${CND_CONF}.mk ESPEngine/ESPVolumeEmitter.cpp 
	${MKDIR} -p ${OBJECTDIR}/ESPEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/ESPEngine/ESPVolumeEmitter.o ESPEngine/ESPVolumeEmitter.cpp

${OBJECTDIR}/include/physfs/dir.o: nbproject/Makefile-${CND_CONF}.mk include/physfs/dir.c 
	${MKDIR} -p ${OBJECTDIR}/include/physfs
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/physfs/dir.o include/physfs/dir.c

${OBJECTDIR}/include/zlib/compress.o: nbproject/Makefile-${CND_CONF}.mk include/zlib/compress.c 
	${MKDIR} -p ${OBJECTDIR}/include/zlib
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/zlib/compress.o include/zlib/compress.c

${OBJECTDIR}/GameView/GameCompleteDisplayState.o: nbproject/Makefile-${CND_CONF}.mk GameView/GameCompleteDisplayState.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/GameCompleteDisplayState.o GameView/GameCompleteDisplayState.cpp

${OBJECTDIR}/BlammoEngine/Skybox.o: nbproject/Makefile-${CND_CONF}.mk BlammoEngine/Skybox.cpp 
	${MKDIR} -p ${OBJECTDIR}/BlammoEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/BlammoEngine/Skybox.o BlammoEngine/Skybox.cpp

${OBJECTDIR}/GameView/FlashHUD.o: nbproject/Makefile-${CND_CONF}.mk GameView/FlashHUD.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/FlashHUD.o GameView/FlashHUD.cpp

${OBJECTDIR}/GameModel/GameOverState.o: nbproject/Makefile-${CND_CONF}.mk GameModel/GameOverState.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/GameOverState.o GameModel/GameOverState.cpp

${OBJECTDIR}/include/physfs/windows.o: nbproject/Makefile-${CND_CONF}.mk include/physfs/windows.c 
	${MKDIR} -p ${OBJECTDIR}/include/physfs
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/physfs/windows.o include/physfs/windows.c

${OBJECTDIR}/BlammoEngine/TextLabel.o: nbproject/Makefile-${CND_CONF}.mk BlammoEngine/TextLabel.cpp 
	${MKDIR} -p ${OBJECTDIR}/BlammoEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/BlammoEngine/TextLabel.o BlammoEngine/TextLabel.cpp

${OBJECTDIR}/GameModel/LaserBeamPaddleItem.o: nbproject/Makefile-${CND_CONF}.mk GameModel/LaserBeamPaddleItem.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/LaserBeamPaddleItem.o GameModel/LaserBeamPaddleItem.cpp

${OBJECTDIR}/GameModel/GameLevel.o: nbproject/Makefile-${CND_CONF}.mk GameModel/GameLevel.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/GameLevel.o GameModel/GameLevel.cpp

${OBJECTDIR}/include/physfs/posix.o: nbproject/Makefile-${CND_CONF}.mk include/physfs/posix.c 
	${MKDIR} -p ${OBJECTDIR}/include/physfs
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/physfs/posix.o include/physfs/posix.c

${OBJECTDIR}/GameView/InGameMenuState.o: nbproject/Makefile-${CND_CONF}.mk GameView/InGameMenuState.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/InGameMenuState.o GameView/InGameMenuState.cpp

${OBJECTDIR}/ESPEngine/ESPParticle.o: nbproject/Makefile-${CND_CONF}.mk ESPEngine/ESPParticle.cpp 
	${MKDIR} -p ${OBJECTDIR}/ESPEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/ESPEngine/ESPParticle.o ESPEngine/ESPParticle.cpp

${OBJECTDIR}/GameView/CgFxPostUberIntense.o: nbproject/Makefile-${CND_CONF}.mk GameView/CgFxPostUberIntense.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/CgFxPostUberIntense.o GameView/CgFxPostUberIntense.cpp

${OBJECTDIR}/ESPEngine/ESPParticleScaleEffector.o: nbproject/Makefile-${CND_CONF}.mk ESPEngine/ESPParticleScaleEffector.cpp 
	${MKDIR} -p ${OBJECTDIR}/ESPEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/ESPEngine/ESPParticleScaleEffector.o ESPEngine/ESPParticleScaleEffector.cpp

${OBJECTDIR}/include/physfs/zip.o: nbproject/Makefile-${CND_CONF}.mk include/physfs/zip.c 
	${MKDIR} -p ${OBJECTDIR}/include/physfs
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/physfs/zip.o include/physfs/zip.c

${OBJECTDIR}/GameSound/Sound.o: nbproject/Makefile-${CND_CONF}.mk GameSound/Sound.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameSound
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameSound/Sound.o GameSound/Sound.cpp

${OBJECTDIR}/GameModel/PaddleLaser.o: nbproject/Makefile-${CND_CONF}.mk GameModel/PaddleLaser.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/PaddleLaser.o GameModel/PaddleLaser.cpp

${OBJECTDIR}/BlammoEngine/ObjReader.o: nbproject/Makefile-${CND_CONF}.mk BlammoEngine/ObjReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/BlammoEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/BlammoEngine/ObjReader.o BlammoEngine/ObjReader.cpp

${OBJECTDIR}/GameView/LaserPaddleGun.o: nbproject/Makefile-${CND_CONF}.mk GameView/LaserPaddleGun.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/LaserPaddleGun.o GameView/LaserPaddleGun.cpp

${OBJECTDIR}/GameModel/BallDeathState.o: nbproject/Makefile-${CND_CONF}.mk GameModel/BallDeathState.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/BallDeathState.o GameModel/BallDeathState.cpp

${OBJECTDIR}/GameModel/PrismBlock.o: nbproject/Makefile-${CND_CONF}.mk GameModel/PrismBlock.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/PrismBlock.o GameModel/PrismBlock.cpp

${OBJECTDIR}/ResourceManager.o: nbproject/Makefile-${CND_CONF}.mk ResourceManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/ResourceManager.o ResourceManager.cpp

${OBJECTDIR}/GameModel/UpsideDownItem.o: nbproject/Makefile-${CND_CONF}.mk GameModel/UpsideDownItem.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/UpsideDownItem.o GameModel/UpsideDownItem.cpp

${OBJECTDIR}/ESPEngine/ESPMultiColourEffector.o: nbproject/Makefile-${CND_CONF}.mk ESPEngine/ESPMultiColourEffector.cpp 
	${MKDIR} -p ${OBJECTDIR}/ESPEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/ESPEngine/ESPMultiColourEffector.o ESPEngine/ESPMultiColourEffector.cpp

${OBJECTDIR}/GameView/CgFxInkBlock.o: nbproject/Makefile-${CND_CONF}.mk GameView/CgFxInkBlock.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/CgFxInkBlock.o GameView/CgFxInkBlock.cpp

${OBJECTDIR}/GameModel/BallCamItem.o: nbproject/Makefile-${CND_CONF}.mk GameModel/BallCamItem.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/BallCamItem.o GameModel/BallCamItem.cpp

${OBJECTDIR}/include/zlib/inflate.o: nbproject/Makefile-${CND_CONF}.mk include/zlib/inflate.c 
	${MKDIR} -p ${OBJECTDIR}/include/zlib
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/zlib/inflate.o include/zlib/inflate.c

${OBJECTDIR}/include/zlib/inffast.o: nbproject/Makefile-${CND_CONF}.mk include/zlib/inffast.c 
	${MKDIR} -p ${OBJECTDIR}/include/zlib
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/zlib/inffast.o include/zlib/inffast.c

${OBJECTDIR}/ESPEngine/ESPUtil.o: nbproject/Makefile-${CND_CONF}.mk ESPEngine/ESPUtil.cpp 
	${MKDIR} -p ${OBJECTDIR}/ESPEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/ESPEngine/ESPUtil.o ESPEngine/ESPUtil.cpp

${OBJECTDIR}/GameView/PrismBlockMesh.o: nbproject/Makefile-${CND_CONF}.mk GameView/PrismBlockMesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/PrismBlockMesh.o GameView/PrismBlockMesh.cpp

${OBJECTDIR}/GameSound/GameSoundAssets.o: nbproject/Makefile-${CND_CONF}.mk GameSound/GameSoundAssets.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameSound
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameSound/GameSoundAssets.o GameSound/GameSoundAssets.cpp

${OBJECTDIR}/GameModel/BallInPlayState.o: nbproject/Makefile-${CND_CONF}.mk GameModel/BallInPlayState.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/BallInPlayState.o GameModel/BallInPlayState.cpp

${OBJECTDIR}/GameModel/LevelPiece.o: nbproject/Makefile-${CND_CONF}.mk GameModel/LevelPiece.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/LevelPiece.o GameModel/LevelPiece.cpp

${OBJECTDIR}/GameModel/PaddleRocketProjectile.o: nbproject/Makefile-${CND_CONF}.mk GameModel/PaddleRocketProjectile.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/PaddleRocketProjectile.o GameModel/PaddleRocketProjectile.cpp

${OBJECTDIR}/BlammoEngine/Camera.o: nbproject/Makefile-${CND_CONF}.mk BlammoEngine/Camera.cpp 
	${MKDIR} -p ${OBJECTDIR}/BlammoEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/BlammoEngine/Camera.o BlammoEngine/Camera.cpp

${OBJECTDIR}/GameModel/GameTransformMgr.o: nbproject/Makefile-${CND_CONF}.mk GameModel/GameTransformMgr.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/GameTransformMgr.o GameModel/GameTransformMgr.cpp

${OBJECTDIR}/BlammoEngine/Point.o: nbproject/Makefile-${CND_CONF}.mk BlammoEngine/Point.cpp 
	${MKDIR} -p ${OBJECTDIR}/BlammoEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/BlammoEngine/Point.o BlammoEngine/Point.cpp

${OBJECTDIR}/GameModel/StickyPaddleItem.o: nbproject/Makefile-${CND_CONF}.mk GameModel/StickyPaddleItem.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/StickyPaddleItem.o GameModel/StickyPaddleItem.cpp

${OBJECTDIR}/GameModel/BallSafetyNetItem.o: nbproject/Makefile-${CND_CONF}.mk GameModel/BallSafetyNetItem.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/BallSafetyNetItem.o GameModel/BallSafetyNetItem.cpp

${OBJECTDIR}/GameModel/OneUpItem.o: nbproject/Makefile-${CND_CONF}.mk GameModel/OneUpItem.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/OneUpItem.o GameModel/OneUpItem.cpp

${OBJECTDIR}/BlammoEngine/mtrand.o: nbproject/Makefile-${CND_CONF}.mk BlammoEngine/mtrand.cpp 
	${MKDIR} -p ${OBJECTDIR}/BlammoEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/BlammoEngine/mtrand.o BlammoEngine/mtrand.cpp

${OBJECTDIR}/GameView/LevelMesh.o: nbproject/Makefile-${CND_CONF}.mk GameView/LevelMesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/LevelMesh.o GameView/LevelMesh.cpp

${OBJECTDIR}/GameModel/CannonBlock.o: nbproject/Makefile-${CND_CONF}.mk GameModel/CannonBlock.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/CannonBlock.o GameModel/CannonBlock.cpp

${OBJECTDIR}/ESPEngine/ESPOnomataParticle.o: nbproject/Makefile-${CND_CONF}.mk ESPEngine/ESPOnomataParticle.cpp 
	${MKDIR} -p ${OBJECTDIR}/ESPEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/ESPEngine/ESPOnomataParticle.o ESPEngine/ESPOnomataParticle.cpp

${OBJECTDIR}/GameModel/CollateralBlock.o: nbproject/Makefile-${CND_CONF}.mk GameModel/CollateralBlock.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/CollateralBlock.o GameModel/CollateralBlock.cpp

${OBJECTDIR}/GameView/MainMenuDisplayState.o: nbproject/Makefile-${CND_CONF}.mk GameView/MainMenuDisplayState.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/MainMenuDisplayState.o GameView/MainMenuDisplayState.cpp

${OBJECTDIR}/GameController.o: nbproject/Makefile-${CND_CONF}.mk GameController.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameController.o GameController.cpp

${OBJECTDIR}/GameView/CgFxGaussianBlur.o: nbproject/Makefile-${CND_CONF}.mk GameView/CgFxGaussianBlur.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/CgFxGaussianBlur.o GameView/CgFxGaussianBlur.cpp

${OBJECTDIR}/BlammoEngine/Texture3D.o: nbproject/Makefile-${CND_CONF}.mk BlammoEngine/Texture3D.cpp 
	${MKDIR} -p ${OBJECTDIR}/BlammoEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/BlammoEngine/Texture3D.o BlammoEngine/Texture3D.cpp

${OBJECTDIR}/GameView/GameDisplay.o: nbproject/Makefile-${CND_CONF}.mk GameView/GameDisplay.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/GameDisplay.o GameView/GameDisplay.cpp

${OBJECTDIR}/BlammoEngine/Texture1D.o: nbproject/Makefile-${CND_CONF}.mk BlammoEngine/Texture1D.cpp 
	${MKDIR} -p ${OBJECTDIR}/BlammoEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/BlammoEngine/Texture1D.o BlammoEngine/Texture1D.cpp

${OBJECTDIR}/include/physfs/beos.o: nbproject/Makefile-${CND_CONF}.mk include/physfs/beos.cpp 
	${MKDIR} -p ${OBJECTDIR}/include/physfs
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/physfs/beos.o include/physfs/beos.cpp

${OBJECTDIR}/GameModel/GameItemFactory.o: nbproject/Makefile-${CND_CONF}.mk GameModel/GameItemFactory.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/GameItemFactory.o GameModel/GameItemFactory.cpp

${OBJECTDIR}/GameModel/BombBlock.o: nbproject/Makefile-${CND_CONF}.mk GameModel/BombBlock.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/BombBlock.o GameModel/BombBlock.cpp

${OBJECTDIR}/include/zlib/uncompr.o: nbproject/Makefile-${CND_CONF}.mk include/zlib/uncompr.c 
	${MKDIR} -p ${OBJECTDIR}/include/zlib
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/zlib/uncompr.o include/zlib/uncompr.c

${OBJECTDIR}/GameModel/UberBallItem.o: nbproject/Makefile-${CND_CONF}.mk GameModel/UberBallItem.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/UberBallItem.o GameModel/UberBallItem.cpp

${OBJECTDIR}/BlammoEngine/Algebra.o: nbproject/Makefile-${CND_CONF}.mk BlammoEngine/Algebra.cpp 
	${MKDIR} -p ${OBJECTDIR}/BlammoEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/BlammoEngine/Algebra.o BlammoEngine/Algebra.cpp

${OBJECTDIR}/GameView/LoadingScreen.o: nbproject/Makefile-${CND_CONF}.mk GameView/LoadingScreen.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/LoadingScreen.o GameView/LoadingScreen.cpp

${OBJECTDIR}/GameView/LivesLeftHUD.o: nbproject/Makefile-${CND_CONF}.mk GameView/LivesLeftHUD.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/LivesLeftHUD.o GameView/LivesLeftHUD.cpp

${OBJECTDIR}/GameModel/PoisonPaddleItem.o: nbproject/Makefile-${CND_CONF}.mk GameModel/PoisonPaddleItem.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/PoisonPaddleItem.o GameModel/PoisonPaddleItem.cpp

${OBJECTDIR}/GameModel/InkBlock.o: nbproject/Makefile-${CND_CONF}.mk GameModel/InkBlock.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/InkBlock.o GameModel/InkBlock.cpp

${OBJECTDIR}/GameModel/BallOnPaddleState.o: nbproject/Makefile-${CND_CONF}.mk GameModel/BallOnPaddleState.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/BallOnPaddleState.o GameModel/BallOnPaddleState.cpp

${OBJECTDIR}/GameView/CgFxFullscreenGoo.o: nbproject/Makefile-${CND_CONF}.mk GameView/CgFxFullscreenGoo.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/CgFxFullscreenGoo.o GameView/CgFxFullscreenGoo.cpp

${OBJECTDIR}/GameModel/PaddleSizeItem.o: nbproject/Makefile-${CND_CONF}.mk GameModel/PaddleSizeItem.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/PaddleSizeItem.o GameModel/PaddleSizeItem.cpp

${OBJECTDIR}/BlammoEngine/MtlReader.o: nbproject/Makefile-${CND_CONF}.mk BlammoEngine/MtlReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/BlammoEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/BlammoEngine/MtlReader.o BlammoEngine/MtlReader.cpp

${OBJECTDIR}/GameView/GameAssets.o: nbproject/Makefile-${CND_CONF}.mk GameView/GameAssets.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/GameAssets.o GameView/GameAssets.cpp

${OBJECTDIR}/GameModel/WorldCompleteState.o: nbproject/Makefile-${CND_CONF}.mk GameModel/WorldCompleteState.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/WorldCompleteState.o GameModel/WorldCompleteState.cpp

${OBJECTDIR}/GameModel/GameCompleteState.o: nbproject/Makefile-${CND_CONF}.mk GameModel/GameCompleteState.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/GameCompleteState.o GameModel/GameCompleteState.cpp

${OBJECTDIR}/GameView/GameMenu.o: nbproject/Makefile-${CND_CONF}.mk GameView/GameMenu.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/GameMenu.o GameView/GameMenu.cpp

${OBJECTDIR}/GameSound/GameSound.o: nbproject/Makefile-${CND_CONF}.mk GameSound/GameSound.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameSound
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameSound/GameSound.o GameSound/GameSound.cpp

${OBJECTDIR}/GameView/GameLightAssets.o: nbproject/Makefile-${CND_CONF}.mk GameView/GameLightAssets.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/GameLightAssets.o GameView/GameLightAssets.cpp

${OBJECTDIR}/GameView/CgFxDecoSkybox.o: nbproject/Makefile-${CND_CONF}.mk GameView/CgFxDecoSkybox.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/CgFxDecoSkybox.o GameView/CgFxDecoSkybox.cpp

${OBJECTDIR}/include/physfs/os2.o: nbproject/Makefile-${CND_CONF}.mk include/physfs/os2.c 
	${MKDIR} -p ${OBJECTDIR}/include/physfs
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/physfs/os2.o include/physfs/os2.c

${OBJECTDIR}/ESPEngine/ESPParticleColourEffector.o: nbproject/Makefile-${CND_CONF}.mk ESPEngine/ESPParticleColourEffector.cpp 
	${MKDIR} -p ${OBJECTDIR}/ESPEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/ESPEngine/ESPParticleColourEffector.o ESPEngine/ESPParticleColourEffector.cpp

${OBJECTDIR}/GameModel/InvisiBallItem.o: nbproject/Makefile-${CND_CONF}.mk GameModel/InvisiBallItem.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/InvisiBallItem.o GameModel/InvisiBallItem.cpp

${OBJECTDIR}/GameView/CrosshairLaserHUD.o: nbproject/Makefile-${CND_CONF}.mk GameView/CrosshairLaserHUD.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/CrosshairLaserHUD.o GameView/CrosshairLaserHUD.cpp

${OBJECTDIR}/include/physfs/pocketpc.o: nbproject/Makefile-${CND_CONF}.mk include/physfs/pocketpc.c 
	${MKDIR} -p ${OBJECTDIR}/include/physfs
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/physfs/pocketpc.o include/physfs/pocketpc.c

${OBJECTDIR}/GameModel/GameEventManager.o: nbproject/Makefile-${CND_CONF}.mk GameModel/GameEventManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/GameEventManager.o GameModel/GameEventManager.cpp

${OBJECTDIR}/include/zlib/deflate.o: nbproject/Makefile-${CND_CONF}.mk include/zlib/deflate.c 
	${MKDIR} -p ${OBJECTDIR}/include/zlib
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/zlib/deflate.o include/zlib/deflate.c

${OBJECTDIR}/GameModel/RocketPaddleItem.o: nbproject/Makefile-${CND_CONF}.mk GameModel/RocketPaddleItem.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/RocketPaddleItem.o GameModel/RocketPaddleItem.cpp

${OBJECTDIR}/GameModel/InCannonBallState.o: nbproject/Makefile-${CND_CONF}.mk GameModel/InCannonBallState.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/InCannonBallState.o GameModel/InCannonBallState.cpp

${OBJECTDIR}/GameView/PlayerHurtHUD.o: nbproject/Makefile-${CND_CONF}.mk GameView/PlayerHurtHUD.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/PlayerHurtHUD.o GameView/PlayerHurtHUD.cpp

${OBJECTDIR}/GameView/DecoSkybox.o: nbproject/Makefile-${CND_CONF}.mk GameView/DecoSkybox.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/DecoSkybox.o GameView/DecoSkybox.cpp

${OBJECTDIR}/GameView/CgFxPortalBlock.o: nbproject/Makefile-${CND_CONF}.mk GameView/CgFxPortalBlock.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/CgFxPortalBlock.o GameView/CgFxPortalBlock.cpp

${OBJECTDIR}/GameView/GameEventsListener.o: nbproject/Makefile-${CND_CONF}.mk GameView/GameEventsListener.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/GameEventsListener.o GameView/GameEventsListener.cpp

${OBJECTDIR}/GameModel/LaserPaddleItem.o: nbproject/Makefile-${CND_CONF}.mk GameModel/LaserPaddleItem.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/LaserPaddleItem.o GameModel/LaserPaddleItem.cpp

${OBJECTDIR}/GameView/CgFxPostSmokey.o: nbproject/Makefile-${CND_CONF}.mk GameView/CgFxPostSmokey.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/CgFxPostSmokey.o GameView/CgFxPostSmokey.cpp

${OBJECTDIR}/GameView/CgFxPostRefract.o: nbproject/Makefile-${CND_CONF}.mk GameView/CgFxPostRefract.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/CgFxPostRefract.o GameView/CgFxPostRefract.cpp

${OBJECTDIR}/GameModel/GameState.o: nbproject/Makefile-${CND_CONF}.mk GameModel/GameState.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/GameState.o GameModel/GameState.cpp

${OBJECTDIR}/BlammoEngine/Noise.o: nbproject/Makefile-${CND_CONF}.mk BlammoEngine/Noise.cpp 
	${MKDIR} -p ${OBJECTDIR}/BlammoEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/BlammoEngine/Noise.o BlammoEngine/Noise.cpp

${OBJECTDIR}/GameModel/GameBall.o: nbproject/Makefile-${CND_CONF}.mk GameModel/GameBall.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/GameBall.o GameModel/GameBall.cpp

${OBJECTDIR}/GameView/PaddleRocketMesh.o: nbproject/Makefile-${CND_CONF}.mk GameView/PaddleRocketMesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/PaddleRocketMesh.o GameView/PaddleRocketMesh.cpp

${OBJECTDIR}/GameModel/GameItemTimer.o: nbproject/Makefile-${CND_CONF}.mk GameModel/GameItemTimer.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/GameItemTimer.o GameModel/GameItemTimer.cpp

${OBJECTDIR}/GameModel/GameModel.o: nbproject/Makefile-${CND_CONF}.mk GameModel/GameModel.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/GameModel.o GameModel/GameModel.cpp

${OBJECTDIR}/GameView/StickyPaddleGoo.o: nbproject/Makefile-${CND_CONF}.mk GameView/StickyPaddleGoo.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/StickyPaddleGoo.o GameView/StickyPaddleGoo.cpp

${OBJECTDIR}/GameView/BallSafetyNetMesh.o: nbproject/Makefile-${CND_CONF}.mk GameView/BallSafetyNetMesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/BallSafetyNetMesh.o GameView/BallSafetyNetMesh.cpp

${OBJECTDIR}/GameView/GameFontAssetsManager.o: nbproject/Makefile-${CND_CONF}.mk GameView/GameFontAssetsManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/GameFontAssetsManager.o GameView/GameFontAssetsManager.cpp

${OBJECTDIR}/ESPEngine/ESPParticleBatchMesh.o: nbproject/Makefile-${CND_CONF}.mk ESPEngine/ESPParticleBatchMesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/ESPEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/ESPEngine/ESPParticleBatchMesh.o ESPEngine/ESPParticleBatchMesh.cpp

${OBJECTDIR}/GameModel/Beam.o: nbproject/Makefile-${CND_CONF}.mk GameModel/Beam.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/Beam.o GameModel/Beam.cpp

${OBJECTDIR}/BlammoEngine/TextureCube.o: nbproject/Makefile-${CND_CONF}.mk BlammoEngine/TextureCube.cpp 
	${MKDIR} -p ${OBJECTDIR}/BlammoEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/BlammoEngine/TextureCube.o BlammoEngine/TextureCube.cpp

${OBJECTDIR}/GameView/GameMenuItem.o: nbproject/Makefile-${CND_CONF}.mk GameView/GameMenuItem.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/GameMenuItem.o GameView/GameMenuItem.cpp

${OBJECTDIR}/include/physfs/physfs.o: nbproject/Makefile-${CND_CONF}.mk include/physfs/physfs.c 
	${MKDIR} -p ${OBJECTDIR}/include/physfs
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/physfs/physfs.o include/physfs/physfs.c

${OBJECTDIR}/GameModel/GameItem.o: nbproject/Makefile-${CND_CONF}.mk GameModel/GameItem.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/GameItem.o GameModel/GameItem.cpp

${OBJECTDIR}/GameModel/BlackoutItem.o: nbproject/Makefile-${CND_CONF}.mk GameModel/BlackoutItem.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/BlackoutItem.o GameModel/BlackoutItem.cpp

${OBJECTDIR}/GameView/CgFxBloom.o: nbproject/Makefile-${CND_CONF}.mk GameView/CgFxBloom.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/CgFxBloom.o GameView/CgFxBloom.cpp

${OBJECTDIR}/GameModel/LevelCompleteState.o: nbproject/Makefile-${CND_CONF}.mk GameModel/LevelCompleteState.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/LevelCompleteState.o GameModel/LevelCompleteState.cpp

${OBJECTDIR}/BlammoEngine/CgFxEffect.o: nbproject/Makefile-${CND_CONF}.mk BlammoEngine/CgFxEffect.cpp 
	${MKDIR} -p ${OBJECTDIR}/BlammoEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/BlammoEngine/CgFxEffect.o BlammoEngine/CgFxEffect.cpp

${OBJECTDIR}/GameModel/GhostBallItem.o: nbproject/Makefile-${CND_CONF}.mk GameModel/GhostBallItem.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/GhostBallItem.o GameModel/GhostBallItem.cpp

${OBJECTDIR}/ESPEngine/ESPShaderParticle.o: nbproject/Makefile-${CND_CONF}.mk ESPEngine/ESPShaderParticle.cpp 
	${MKDIR} -p ${OBJECTDIR}/ESPEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/ESPEngine/ESPShaderParticle.o ESPEngine/ESPShaderParticle.cpp

${OBJECTDIR}/include/zlib/gzio.o: nbproject/Makefile-${CND_CONF}.mk include/zlib/gzio.c 
	${MKDIR} -p ${OBJECTDIR}/include/zlib
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/zlib/gzio.o include/zlib/gzio.c

${OBJECTDIR}/include/physfs/physfs_byteorder.o: nbproject/Makefile-${CND_CONF}.mk include/physfs/physfs_byteorder.c 
	${MKDIR} -p ${OBJECTDIR}/include/physfs
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/physfs/physfs_byteorder.o include/physfs/physfs_byteorder.c

${OBJECTDIR}/GameView/GameESPAssets.o: nbproject/Makefile-${CND_CONF}.mk GameView/GameESPAssets.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/GameESPAssets.o GameView/GameESPAssets.cpp

${OBJECTDIR}/include/zlib/zutil.o: nbproject/Makefile-${CND_CONF}.mk include/zlib/zutil.c 
	${MKDIR} -p ${OBJECTDIR}/include/zlib
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/zlib/zutil.o include/zlib/zutil.c

${OBJECTDIR}/BlammoEngine/TextureFontSet.o: nbproject/Makefile-${CND_CONF}.mk BlammoEngine/TextureFontSet.cpp 
	${MKDIR} -p ${OBJECTDIR}/BlammoEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/BlammoEngine/TextureFontSet.o BlammoEngine/TextureFontSet.cpp

${OBJECTDIR}/GameView/CgFxPhong.o: nbproject/Makefile-${CND_CONF}.mk GameView/CgFxPhong.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/CgFxPhong.o GameView/CgFxPhong.cpp

${OBJECTDIR}/BlammoEngine/GeometryMaker.o: nbproject/Makefile-${CND_CONF}.mk BlammoEngine/GeometryMaker.cpp 
	${MKDIR} -p ${OBJECTDIR}/BlammoEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/BlammoEngine/GeometryMaker.o BlammoEngine/GeometryMaker.cpp

${OBJECTDIR}/GameView/GameViewConstants.o: nbproject/Makefile-${CND_CONF}.mk GameView/GameViewConstants.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/GameViewConstants.o GameView/GameViewConstants.cpp

${OBJECTDIR}/GameView/GameOverDisplayState.o: nbproject/Makefile-${CND_CONF}.mk GameView/GameOverDisplayState.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/GameOverDisplayState.o GameView/GameOverDisplayState.cpp

${OBJECTDIR}/GameView/CollateralBlockMesh.o: nbproject/Makefile-${CND_CONF}.mk GameView/CollateralBlockMesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/CollateralBlockMesh.o GameView/CollateralBlockMesh.cpp

${OBJECTDIR}/GameModel/PlayerPaddle.o: nbproject/Makefile-${CND_CONF}.mk GameModel/PlayerPaddle.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/PlayerPaddle.o GameModel/PlayerPaddle.cpp

${OBJECTDIR}/include/physfs/physfs_unicode.o: nbproject/Makefile-${CND_CONF}.mk include/physfs/physfs_unicode.c 
	${MKDIR} -p ${OBJECTDIR}/include/physfs
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/physfs/physfs_unicode.o include/physfs/physfs_unicode.c

${OBJECTDIR}/BlammoEngine/Texture.o: nbproject/Makefile-${CND_CONF}.mk BlammoEngine/Texture.cpp 
	${MKDIR} -p ${OBJECTDIR}/BlammoEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/BlammoEngine/Texture.o BlammoEngine/Texture.cpp

${OBJECTDIR}/GameModel/TriangleBlocks.o: nbproject/Makefile-${CND_CONF}.mk GameModel/TriangleBlocks.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/TriangleBlocks.o GameModel/TriangleBlocks.cpp

${OBJECTDIR}/GameView/StartGameDisplayState.o: nbproject/Makefile-${CND_CONF}.mk GameView/StartGameDisplayState.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/StartGameDisplayState.o GameView/StartGameDisplayState.cpp

${OBJECTDIR}/BlammoEngine/BlammoTime.o: nbproject/Makefile-${CND_CONF}.mk BlammoEngine/BlammoTime.cpp 
	${MKDIR} -p ${OBJECTDIR}/BlammoEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/BlammoEngine/BlammoTime.o BlammoEngine/BlammoTime.cpp

${OBJECTDIR}/GameSound/MSFReader.o: nbproject/Makefile-${CND_CONF}.mk GameSound/MSFReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameSound
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameSound/MSFReader.o GameSound/MSFReader.cpp

${OBJECTDIR}/GameView/PortalBlockMesh.o: nbproject/Makefile-${CND_CONF}.mk GameView/PortalBlockMesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/PortalBlockMesh.o GameView/PortalBlockMesh.cpp

${OBJECTDIR}/GameView/CgFxInkSplatter.o: nbproject/Makefile-${CND_CONF}.mk GameView/CgFxInkSplatter.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/CgFxInkSplatter.o GameView/CgFxInkSplatter.cpp

${OBJECTDIR}/GameView/GameFBOAssets.o: nbproject/Makefile-${CND_CONF}.mk GameView/GameFBOAssets.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/GameFBOAssets.o GameView/GameFBOAssets.cpp

${OBJECTDIR}/GameModel/PortalBlock.o: nbproject/Makefile-${CND_CONF}.mk GameModel/PortalBlock.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/PortalBlock.o GameModel/PortalBlock.cpp

${OBJECTDIR}/ESPEngine/ESPPointEmitter.o: nbproject/Makefile-${CND_CONF}.mk ESPEngine/ESPPointEmitter.cpp 
	${MKDIR} -p ${OBJECTDIR}/ESPEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/ESPEngine/ESPPointEmitter.o ESPEngine/ESPPointEmitter.cpp

${OBJECTDIR}/WindowManager.o: nbproject/Makefile-${CND_CONF}.mk WindowManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/WindowManager.o WindowManager.cpp

${OBJECTDIR}/GameView/CgFxCelShading.o: nbproject/Makefile-${CND_CONF}.mk GameView/CgFxCelShading.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/CgFxCelShading.o GameView/CgFxCelShading.cpp

${OBJECTDIR}/GameModel/BallSpeedItem.o: nbproject/Makefile-${CND_CONF}.mk GameModel/BallSpeedItem.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/BallSpeedItem.o GameModel/BallSpeedItem.cpp

${OBJECTDIR}/GameMain.o: nbproject/Makefile-${CND_CONF}.mk GameMain.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameMain.o GameMain.cpp

${OBJECTDIR}/include/physfs/unix.o: nbproject/Makefile-${CND_CONF}.mk include/physfs/unix.c 
	${MKDIR} -p ${OBJECTDIR}/include/physfs
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/physfs/unix.o include/physfs/unix.c

${OBJECTDIR}/GameModel/Projectile.o: nbproject/Makefile-${CND_CONF}.mk GameModel/Projectile.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/Projectile.o GameModel/Projectile.cpp

${OBJECTDIR}/GameView/CgFxAfterImage.o: nbproject/Makefile-${CND_CONF}.mk GameView/CgFxAfterImage.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/CgFxAfterImage.o GameView/CgFxAfterImage.cpp

${OBJECTDIR}/GameView/CannonBlockMesh.o: nbproject/Makefile-${CND_CONF}.mk GameView/CannonBlockMesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/CannonBlockMesh.o GameView/CannonBlockMesh.cpp

${OBJECTDIR}/GameModel/GravityBallItem.o: nbproject/Makefile-${CND_CONF}.mk GameModel/GravityBallItem.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/GravityBallItem.o GameModel/GravityBallItem.cpp

${OBJECTDIR}/include/physfs/macosx.o: nbproject/Makefile-${CND_CONF}.mk include/physfs/macosx.c 
	${MKDIR} -p ${OBJECTDIR}/include/physfs
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/physfs/macosx.o include/physfs/macosx.c

${OBJECTDIR}/ConfigOptions.o: nbproject/Makefile-${CND_CONF}.mk ConfigOptions.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/ConfigOptions.o ConfigOptions.cpp

${OBJECTDIR}/GameView/DecoWorldAssets.o: nbproject/Makefile-${CND_CONF}.mk GameView/DecoWorldAssets.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/DecoWorldAssets.o GameView/DecoWorldAssets.cpp

${OBJECTDIR}/include/SDLMain.o: nbproject/Makefile-${CND_CONF}.mk include/SDLMain.m 
	${MKDIR} -p ${OBJECTDIR}/include
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/SDLMain.o include/SDLMain.m

${OBJECTDIR}/ESPEngine/ESPLineEmitter.o: nbproject/Makefile-${CND_CONF}.mk ESPEngine/ESPLineEmitter.cpp 
	${MKDIR} -p ${OBJECTDIR}/ESPEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/ESPEngine/ESPLineEmitter.o ESPEngine/ESPLineEmitter.cpp

${OBJECTDIR}/GameView/CgFxVolumetricEffect.o: nbproject/Makefile-${CND_CONF}.mk GameView/CgFxVolumetricEffect.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/CgFxVolumetricEffect.o GameView/CgFxVolumetricEffect.cpp

${OBJECTDIR}/GameModel/GameModelConstants.o: nbproject/Makefile-${CND_CONF}.mk GameModel/GameModelConstants.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/GameModelConstants.o GameModel/GameModelConstants.cpp

${OBJECTDIR}/GameView/InGameDisplayState.o: nbproject/Makefile-${CND_CONF}.mk GameView/InGameDisplayState.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameView
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameView/InGameDisplayState.o GameView/InGameDisplayState.cpp

${OBJECTDIR}/BlammoEngine/VBOBatch.o: nbproject/Makefile-${CND_CONF}.mk BlammoEngine/VBOBatch.cpp 
	${MKDIR} -p ${OBJECTDIR}/BlammoEngine
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/BlammoEngine/VBOBatch.o BlammoEngine/VBOBatch.cpp

${OBJECTDIR}/GameModel/Onomatoplex.o: nbproject/Makefile-${CND_CONF}.mk GameModel/Onomatoplex.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/Onomatoplex.o GameModel/Onomatoplex.cpp

${OBJECTDIR}/GameModel/BreakableBlock.o: nbproject/Makefile-${CND_CONF}.mk GameModel/BreakableBlock.cpp 
	${MKDIR} -p ${OBJECTDIR}/GameModel
	${RM} $@.d
	$(COMPILE.cc) -g -I. -Iinclude -IGameModel -IGameView -IGameSound -IBlammoEngine -IESPEngine -Ilib -Iinclude/physfs -Iinclude/AL -MMD -MP -MF $@.d -o ${OBJECTDIR}/GameModel/BreakableBlock.o GameModel/BreakableBlock.cpp

${OBJECTDIR}/include/zlib/inftrees.o: nbproject/Makefile-${CND_CONF}.mk include/zlib/inftrees.c 
	${MKDIR} -p ${OBJECTDIR}/include/zlib
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/include/zlib/inftrees.o include/zlib/inftrees.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-MacOSX/biffbamblammo

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
