package com.cw.assimpviewer

import android.content.Context
import android.content.res.AssetManager
import android.util.AttributeSet
import android.view.*
import com.almeros.android.multitouch.MoveGestureDetector
import com.almeros.android.multitouch.RotateGestureDetector
import com.almeros.android.multitouch.ShoveGestureDetector

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
        mScaleDetector = ScaleGestureDetector(context.applicationContext, ScaleListener(this))
        mRotateDetector = RotateGestureDetector(context.applicationContext, RotateListener(this))
        mMoveDetector = MoveGestureDetector(context.applicationContext, MoveListener(this))
        mShoveDetector = ShoveGestureDetector(context.applicationContext, ShoveListener(this))
    }

    private var mScaleDetector: ScaleGestureDetector? = null
    private var mRotateDetector: RotateGestureDetector? = null
    private var mMoveDetector: MoveGestureDetector? = null
    private var mShoveDetector: ShoveGestureDetector? = null

    private val screenWidth = resources.displayMetrics.widthPixels
    private val screenHeight = resources.displayMetrics.heightPixels

    private var active = false

    override fun surfaceCreated(holder: SurfaceHolder?) {
        nativeAssimpCreated(holder!!.surface, context.assets)
        Choreographer.getInstance().postFrameCallback(this)
        active = true
        nativeAssimpScrollAsync(
            (objScrollX * 100 / screenWidth * 4).toInt(),
            (objScrollY * 100 / screenHeight * 4).toInt()
        )
        nativeAssimpScaleAsync(objScale.toInt())
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

    override fun onTouchEvent(event: MotionEvent?): Boolean {
        mScaleDetector?.onTouchEvent(event)
        mRotateDetector?.onTouchEvent(event)
        mMoveDetector?.onTouchEvent(event)
        mShoveDetector?.onTouchEvent(event)
        return true
    }

    private inner class ScaleListener(val gestureSurfaceView: NativeAssimpView) :
        ScaleGestureDetector.SimpleOnScaleGestureListener() {
        override fun onScale(detector: ScaleGestureDetector): Boolean {
//            Log.d(TAG, "scale ${detector.scaleFactor}")
            objScale *= detector.scaleFactor
            nativeAssimpScale(objScale.toInt())
            return true
        }
    }

    private var objScale = 1000000f

    private inner class RotateListener(val gestureSurfaceView: NativeAssimpView) :
        RotateGestureDetector.SimpleOnRotateGestureListener() {
        override fun onRotate(detector: RotateGestureDetector): Boolean {
//            Log.d(TAG, "rotate ${detector.rotationDegreesDelta}")
            return false
        }
    }

    private var objScrollX = 0f
    private var objScrollY = 0f

    private inner class MoveListener(val gestureSurfaceView: NativeAssimpView) :
        MoveGestureDetector.SimpleOnMoveGestureListener() {
        override fun onMove(detector: MoveGestureDetector): Boolean {
            objScrollX += detector.focusDelta.x
            objScrollY += detector.focusDelta.y
            nativeAssimpScroll(
                (objScrollX * 100 / screenWidth * 4).toInt(),
                (objScrollY * 100 / screenHeight * 4).toInt()
            )
            return true
        }
    }

    private inner class ShoveListener(val gestureSurfaceView: NativeAssimpView) :
        ShoveGestureDetector.SimpleOnShoveGestureListener() {
        override fun onShove(detector: ShoveGestureDetector): Boolean {
//            Log.d(TAG, "shove ${detector.shovePixelsDelta}")
            return false
        }
    }

    private external fun nativeAssimpCreated(surface: Surface, assetManager: AssetManager)
    private external fun nativeAssimpChanged(width: Int, height: Int)
    private external fun nativeAssimpDestroyed()
    private external fun nativeAssimpDoFrame()
    private external fun nativeAssimpScroll(objScrollX: Int, objScrollY: Int)
    private external fun nativeAssimpScrollAsync(objScrollX: Int, objScrollY: Int)
    private external fun nativeAssimpScale(scale: Int)
    private external fun nativeAssimpScaleAsync(scale: Int)

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }
}