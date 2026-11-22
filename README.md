# Flowtunes

Endless music in the genre you pick, made with a local music model. This fork adds Windows support with an independently running ACE-Step 1.5 API server, plus Whisper Lo-fi: dark, muffled music that sounds like it is playing inside a cardboard box.

**Windows:** See [Windows setup](#windows) for build and server instructions. The original Omarchy Linux integration remains available.

![Flowtunes playing a lo-fi tune, its shape drawn in dots over an aurora](docs/screenshot.png)

Pick a genre and the music never stops. The model makes one tune after another, each a little different from the last, and always has the next two ready behind the one playing, so a skip never waits. Lo-fi, Whisper Lo-fi, Ambient, Synthwave, Deep House, Techno, Drum & Bass, Jazz, Classical, Downtempo, Post-Rock, Acoustic and Cinematic are there to flow to. Turn the lyrics on and the tunes are sung, with words the model writes in your language; leave them off for music alone.

Every tune has a name: a sung one is named for its hook, the line it sings most, and one nobody sings in gets two words of its genre, like Dusty Notebook or Neon Boulevard. The name is kept in the MP3's tag, and the tune in a folder of its genre in your music, like `~/Music/Deep House`, named for when it was made, its tempo and its key, and ending in `-instrumental` when nobody sings in it. A genre that has tunes from before plays those first, in a new order each time, before the model makes more, and any MP3 of your own in the folder plays along. The model runs in [omarchy-local-ai](https://github.com/omacom/omarchy-local-ai), and Flowtunes starts it, or offers to set it up, when it isn't running.

## Shortcuts

| Keys | Action |
| --- | --- |
| Arrows | Move between the genres |
| Enter | Play the genre |
| Space | Pause or play |
| N | Skip to the next tune |
| L | Turn the lyrics on or off |
| G | Pick another genre |
| ? | All shortcuts |
| Q | Quit |

On Linux, the media keys play, pause and skip too, as does anything else that speaks MPRIS, like `playerctl`, and the bar shows the tune that's playing. Previous starts the tune over. Windows system media controls are not yet implemented.

## Command line

```sh
flowtunes            # pick a genre
flowtunes jazz       # start playing one right away
```

## License

MIT.

## Windows

Flowtunes connects to a separately managed [ACE-Step 1.5 API server](https://github.com/ace-step/ACE-Step-1.5/blob/main/docs/en/API.md). Install ACE-Step and its models using its instructions, and ensure **FFmpeg is on the server's PATH** so it can export MP3 files. Start the API server with ACE-Step's `start_api_server.bat`, normally at `http://127.0.0.1:8001`. Alternatively, enable the API on its Gradio interface with `--enable-api`; the regular Gradio interface alone does not expose the task API.

In Flowtunes, use **Server** to change the address if needed, then select a genre. Whisper Lo-fi is a darker, muffled variation. Tracks are saved in the Windows Music folder by genre. If a genre already has tracks, Flowtunes plays those before generating new ones. The server needs its language model for generated lyrics and thinking mode.

The server and Flowtunes can run on separate machines. Authentication is not yet supported; use an unauthenticated server on a trusted network. The public API has no cancellation endpoint, so changing genres or closing Flowtunes discards the result but may leave a task running. FFmpeg on the **Flowtunes** machine is optional and supplies waveform analysis; playback works without it.

### Build and test

Install MSVC 2022, CMake, Ninja and Qt 6.8.3 (including Qt Multimedia and Shader Tools), open an x64 Native Tools PowerShell, then run:

```powershell
./scripts/build-windows.ps1 -QtDir C:/Qt/6.8.3/msvc2022_64
```

The script builds and tests the app, then deploys it to `dist/Flowtunes`. Run `ISCC scripts/flowtunes.iss` with Inno Setup 6 to build an installer. The package includes Qt's multimedia runtime; the build script also copies the MSVC runtime when `VCToolsRedistDir` is set. Windows CI produces a deployable artifact.

Generation and playback have been exercised with ACE-Step 1.5 Turbo and a 1.7B language model on Windows with an RTX 4090. Actual performance and sound vary with the server configuration. Windows system media transport controls are not implemented yet; the in-app controls and shortcuts work.
