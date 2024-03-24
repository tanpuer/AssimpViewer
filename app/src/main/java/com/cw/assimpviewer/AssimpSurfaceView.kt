package com.cw.assimpviewer

import android.content.Context
import android.util.AttributeSet
import android.view.Choreographer
import android.view.MotionEvent
import android.view.ScaleGestureDetector
import android.view.SurfaceHolder
import android.view.SurfaceView
import com.almeros.android.multitouch.MoveGestureDetector
import com.almeros.android.multitouch.RotateGestureDetector
import com.almeros.android.multitouch.ShoveGestureDetector

class AssimpSurfaceView @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null
) : SurfaceView(context, attrs), SurfaceHolder.Callback, Choreographer.FrameCallback {

    init {
        holder.addCallback(this)
    }

    private var mScaleDetector: ScaleGestureDetector =
        ScaleGestureDetector(context.applicationContext, ScaleListener(this))
    private var mRotateDetector: RotateGestureDetector =
        RotateGestureDetector(context.applicationContext, RotateListener(this))
    private var mMoveDetector: MoveGestureDetector =
        MoveGestureDetector(context.applicationContext, MoveListener(this))
    private var mShoveDetector: ShoveGestureDetector =
        ShoveGestureDetector(context.applicationContext, ShoveListener(this))

    private val screenWidth = resources.displayMetrics.widthPixels
    private val screenHeight = resources.displayMetrics.heightPixels

    private var active = false

    private val engine = AssimpEngine()

    override fun surfaceCreated(holder: SurfaceHolder) {
        engine.create(holder.surface)
        Choreographer.getInstance().postFrameCallback(this)
        active = true
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
        engine.change(holder.surface, width, height)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        active = false
        Choreographer.getInstance().removeFrameCallback(this)
        engine.destroy(holder.surface)
    }

    override fun doFrame(frameTimeNanos: Long) {
        if (active) {
            engine.doFrame(frameTimeNanos)
            Choreographer.getInstance().postFrameCallback(this)
        }
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        mScaleDetector.onTouchEvent(event)
        mRotateDetector.onTouchEvent(event)
        mMoveDetector.onTouchEvent(event)
        mShoveDetector.onTouchEvent(event)
        return true
    }

    private inner class ScaleListener(val gestureSurfaceView: AssimpSurfaceView) :
        ScaleGestureDetector.SimpleOnScaleGestureListener() {
        override fun onScale(detector: ScaleGestureDetector): Boolean {
//            Log.d(TAG, "scale ${detector.scaleFactor}")
            objScale *= detector.scaleFactor
            engine.setScale(objScale.toInt())
            return true
        }
    }

    private var objScale = 1000000f

    private inner class RotateListener(val gestureSurfaceView: AssimpSurfaceView) :
        RotateGestureDetector.SimpleOnRotateGestureListener() {
        override fun onRotate(detector: RotateGestureDetector): Boolean {
//            Log.d(TAG, "rotate ${detector.rotationDegreesDelta}")
            return false
        }
    }

    private var objScrollX = 0f
    private var objScrollY = 0f

    private inner class MoveListener(val gestureSurfaceView: AssimpSurfaceView) :
        MoveGestureDetector.SimpleOnMoveGestureListener() {
        override fun onMove(detector: MoveGestureDetector): Boolean {
            objScrollX += detector.focusDelta.x
            objScrollY += detector.focusDelta.y
            engine.setScroll(
                (objScrollX * 100 / screenWidth * 4).toInt(),
                (objScrollY * 100 / screenHeight * 4).toInt()
            )
            return true
        }
    }

    private inner class ShoveListener(val gestureSurfaceView: AssimpSurfaceView) :
        ShoveGestureDetector.SimpleOnShoveGestureListener() {
        override fun onShove(detector: ShoveGestureDetector): Boolean {
//            Log.d(TAG, "shove ${detector.shovePixelsDelta}")
            return false
        }
    }


}