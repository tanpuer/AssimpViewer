package com.cw.assimpviewer

import android.content.res.AssetManager
import android.os.Handler
import android.os.HandlerThread
import android.view.Surface

class AssimpEngine {

    private val glThread = HandlerThread("assimp_viewer", Thread.MAX_PRIORITY).apply {
        start()
    }

    private val glHandler = Handler(glThread.looper)

    init {
        glHandler.post {
            nativeAssimpInit(AssimpApplication.getInstance().assets)
        }
    }

    fun create(surface: Surface) {
        glHandler.post {
            nativeAssimpCreated(surface)
        }
    }

    fun change(surface: Surface, width: Int, height: Int) {
        glHandler.post {
            nativeAssimpChanged(width, height)
        }
    }

    fun destroy(surface: Surface) {
        glHandler.post {
            nativeAssimpDestroyed()
        }
    }

    fun doFrame(timeMills: Long) {
        glHandler.post {
            nativeAssimpDoFrame()
        }
    }

    fun setScroll(objScrollX: Int, objScrollY: Int) {
        glHandler.post {
            nativeAssimpScroll(objScrollX, objScrollY)
        }
    }

    fun setScale(scale: Int) {
        glHandler.post {
            nativeAssimpScale(scale)
        }
    }

    fun release() {
        glHandler.post {
            nativeAssimpRelease()
        }
        glThread.quitSafely()
    }

    private external fun nativeAssimpInit(assetManager: AssetManager)
    private external fun nativeAssimpCreated(surface: Surface)
    private external fun nativeAssimpChanged(width: Int, height: Int)
    private external fun nativeAssimpDestroyed()
    private external fun nativeAssimpDoFrame()
    private external fun nativeAssimpScroll(objScrollX: Int, objScrollY: Int)
    private external fun nativeAssimpScale(scale: Int)
    private external fun nativeAssimpRelease()

    companion object {
        init {
            System.loadLibrary("assimpviewer")
        }
    }

}