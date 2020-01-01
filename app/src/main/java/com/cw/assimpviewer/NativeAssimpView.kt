package com.cw.assimpviewer

import android.content.Context
import android.util.AttributeSet
import android.view.Choreographer
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView

class NativeAssimpView : SurfaceView, SurfaceHolder.Callback, Choreographer.FrameCallback {

    constructor(context: Context?) : super(context)
    constructor(context: Context?, attrs: AttributeSet?) : super(context, attrs)
    constructor(context: Context?, attrs: AttributeSet?, defStyleAttr: Int) : super(
        context,
        attrs,
        defStyleAttr
    )

    init {
        holder.addCallback(this)
    }

    private var active = false

    override fun surfaceCreated(holder: SurfaceHolder?) {
        nativeAssimpCreated(holder!!.surface)
        Choreographer.getInstance().postFrameCallback(this)
        active = true
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
        nativeAssimpChanged(width, height)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        active = false
        Choreographer.getInstance().removeFrameCallback(this)
        nativeAssimpDestroyed()
    }

    override fun doFrame(frameTimeNanos: Long) {
        if (active) {
            nativeAssimpDoFrame()
            Choreographer.getInstance().postFrameCallback(this)
        }
    }

    private external fun nativeAssimpCreated(surface: Surface)
    private external fun nativeAssimpChanged(width: Int, height: Int)
    private external fun nativeAssimpDestroyed()
    private external fun nativeAssimpDoFrame()

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }
}