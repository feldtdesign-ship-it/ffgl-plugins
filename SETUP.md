# FFGL Plugin Repo Setup
Plain English. Follow in order. Do this once.

---

## What this repo does
Every time you push updated plugin code to GitHub, it automatically
builds Mac .bundle files AND Windows .dll files in the cloud.
You download them from the Actions tab. No PC required.

---

## Step 1: Set up the repo structure on your Mac

Your repo folder should look exactly like this:

```
ffgl-plugins/
  .github/
    workflows/
      build.yml
  source/
    RotatingSnakes/
      RotatingSnakes.h
      RotatingSnakes.cpp
    ChromaticDots/
      ChromaticDots.h
      ChromaticDots.cpp
    Ouchi/
      Ouchi.h
      Ouchi.cpp
    WagonWheel/
      WagonWheel.h
      WagonWheel.cpp
    Dreamachine/
      Dreamachine.h
      Dreamachine.cpp
  CMakeLists.txt
  README.md
```

---

## Step 2: Add the FFGL SDK as a submodule

The build system needs the FFGL SDK code. This command links it in
automatically so GitHub can find it when it builds.

Open Terminal, navigate to your repo folder, then run:

```bash
git submodule add https://github.com/resolume/ffgl.git ffgl
git commit -m "add ffgl submodule"
```

This creates an ffgl/ folder inside your repo pointing to the
official Resolume FFGL SDK. You never edit files inside ffgl/.

---

## Step 3: Push everything to GitHub

```bash
git add .
git commit -m "initial plugin build setup"
git push
```

---

## Step 4: Watch the build happen

1. Go to your repo on github.com
2. Click the Actions tab at the top
3. You will see a workflow called "Build FFGL Plugins" running
4. Green checkmark = success. Red X = something failed (message Claude)
5. Click the completed run, then scroll down to Artifacts
6. Download mac-bundles.zip and windows-dlls.zip

---

## Step 5: Every time you update a plugin

Edit your .cpp or .h file on your Mac, then:

```bash
git add .
git commit -m "describe what you changed"
git push
```

GitHub builds both versions automatically. Go to Actions, download
the new artifacts. Done.

---

## Adding a new plugin later

1. Create source/NewPluginName/NewPluginName.h and .cpp
2. Add one line to CMakeLists.txt:
   add_ffgl_plugin(NewPluginName source/NewPluginName/NewPluginName.cpp)
3. Push. GitHub builds it.

---

## If the build fails

Go to Actions tab, click the failed run, click the failing job
(mac or windows), read the red error line. Copy it and send to Claude.
