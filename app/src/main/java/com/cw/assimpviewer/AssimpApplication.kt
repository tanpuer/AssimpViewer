package com.cw.assimpviewer

import android.app.Application

class AssimpApplication : Application() {

    companion object {
        private lateinit var mInstance: AssimpApplication

        fun getInstance(): AssimpApplication {
            return mInstance
        }
    }

    override fun onCreate() {
        super.onCreate()
        mInstance = this
    }

}