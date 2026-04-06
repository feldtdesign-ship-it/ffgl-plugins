#include "ChromaticDots.h"
using namespace ffglqs;

static CFFGLPluginInfo PluginInfo = Source::CreatePlugin< ChromaticDots >( [] {
    ffglqs::PluginInfo i( "MCDO", "Chromatic Dots" );
    i.description  = "Isoluminant chromatic motion and breathing expansion illusion via cycling HSV dot grid";
    i.about        = "Michael Weinfeld / NSGL";
    i.majorVersion = 1;
    i.minorVersion = 0;
    return i;
}() );

static const char fragmentShader[] = R"(

// ---------------------------------------------------------------------------
// Chromatic Dots -- Two optical illusion modes on a static dot grid
//
// MODE 0: Isoluminant Crawl
//   Dot hue and background hue cycle independently at matched luminance.
//   The primary motion system (magnocellular) sees no luminance edge.
//   The third-order motion system fires on chromatic contrast alone,
//   causing the static grid to appear to crawl or vibrate.
//
// MODE 1: Breathing Expansion
//   Background hue cycles continuously through the full HSV spectrum.
//   Dark hues (purple/red) visually attract dot edges inward (shrink).
//   Light hues (yellow/green) push dot edges outward (expand).
//   Physical dot coordinates never change -- only perceived size pulses.
//
// Uniforms (auto-injected): time, resolution, i_uv
// Custom: Mode, GridCount, DotSize, DotHue, DotLuma,
//         BgHue, BgLuma, ChromaSpeed, Ellipse, EllipseAngle
// ---------------------------------------------------------------------------

// HSV to RGB conversion
vec3 hsv2rgb( float h, float s, float v )
{
    float h6 = fract( h ) * 6.0;
    float r  = clamp( abs( h6 - 3.0 ) - 1.0, 0.0, 1.0 );
    float g  = clamp( 2.0 - abs( h6 - 2.0 ), 0.0, 1.0 );
    float b  = clamp( 2.0 - abs( h6 - 4.0 ), 0.0, 1.0 );
    vec3  rgb = vec3( r, g, b );
    // Apply saturation and value
    return v * mix( vec3( 1.0 ), rgb, s );
}

void main()
{
    // Aspect-correct UV space, centered on (0,0)
    float aspect = resolution.x / resolution.y;
    vec2 uv = i_uv;
    uv.x *= aspect;

    // --- Grid construction ---
    // Divide UV space into GridCount x GridCount cells
    vec2 cell     = uv * GridCount;
    vec2 cell_id  = floor( cell );
    vec2 cell_uv  = fract( cell ) - 0.5;  // -0.5 to 0.5, centered in cell

    // --- Ellipse deformation ---
    // EllipseAngle rotates the stretch axis
    float angle = EllipseAngle * 6.28318;
    float ca    = cos( angle );
    float sa    = sin( angle );
    vec2 rot_uv = vec2(
        ca * cell_uv.x - sa * cell_uv.y,
        sa * cell_uv.x + ca * cell_uv.y
    );
    // Ellipse squash: x axis compressed by Ellipse factor
    float squash   = 1.0 + Ellipse * 1.5;
    vec2 ell_uv    = vec2( rot_uv.x * squash, rot_uv.y );

    // Distance from cell center (ellipse-adjusted)
    float dist = length( ell_uv );

    // Dot radius in cell units (0.5 = fills cell edge to edge)
    float radius = DotSize * 0.48;

    // Soft anti-aliased dot edge
    float edge      = 0.012;
    float dot_mask  = 1.0 - smoothstep( radius - edge, radius + edge, dist );

    // --- Mode switch ---
    bool isoluminant = ( Mode < 0.5 );

    // --- Time-driven hue offsets ---
    float t = time * ChromaSpeed;

    vec3 dot_color;
    vec3 bg_color;

    if ( isoluminant )
    {
        // MODE 0: Isoluminant crawl
        // Both dot and background cycle hue at matched luminance.
        // Dot hue shifts slowly, background hue shifts at a slightly
        // different rate to create the chromatic motion signal.
        float dot_hue_cycle = fract( DotHue + t * 0.11 );
        float bg_hue_cycle  = fract( BgHue  + t * 0.17 );

        // Saturation held high, luminance matched between dot and background
        // so the magnocellular (luminance) pathway sees no edge.
        // DotLuma and BgLuma are set equal by the VJ to achieve true isoluminance.
        dot_color = hsv2rgb( dot_hue_cycle, 1.0, DotLuma );
        bg_color  = hsv2rgb( bg_hue_cycle,  1.0, BgLuma  );
    }
    else
    {
        // MODE 1: Breathing expansion
        // Background cycles full HSV spectrum continuously.
        // Dot color is static (controlled by DotHue / DotLuma).
        // The perceptual size of the dot pulses with the background brightness:
        //   dark bg (purple/red) -> edges appear attracted inward -> dot shrinks
        //   light bg (yellow/green) -> edges pushed outward -> dot expands
        float bg_hue_cycle = fract( BgHue + t );
        dot_color = hsv2rgb( DotHue, 1.0, DotLuma );
        bg_color  = hsv2rgb( bg_hue_cycle, 1.0, BgLuma );
    }

    // --- Composite ---
    vec3 color = mix( bg_color, dot_color, dot_mask );

    fragColor = vec4( color, 1.0 );
}
)";

ChromaticDots::ChromaticDots()
{
    SetFragmentShader( fragmentShader );

    // Mode: 0 = Isoluminant crawl, 1 = Breathing expansion
    AddParam( ParamRange::Create( "Mode",         0.0f,  { 0.0f,  1.0f  } ) );

    // Grid
    AddParam( ParamRange::Create( "GridCount",   16.0f,  { 2.0f,  40.0f } ) );
    AddParam( ParamRange::Create( "DotSize",      0.5f,  { 0.0f,  1.0f  } ) );

    // Dot color -- hue and luminance fully independent
    AddParam( ParamRange::Create( "DotHue",       0.0f,  { 0.0f,  1.0f  } ) );
    AddParam( ParamRange::Create( "DotLuma",      0.5f,  { 0.0f,  1.0f  } ) );

    // Background color -- hue and luminance fully independent
    AddParam( ParamRange::Create( "BgHue",        0.5f,  { 0.0f,  1.0f  } ) );
    AddParam( ParamRange::Create( "BgLuma",       0.5f,  { 0.0f,  1.0f  } ) );

    // Chroma cycle speed
    AddParam( ParamRange::Create( "ChromaSpeed",  0.3f,  { 0.0f,  2.0f  } ) );

    // Ellipse deformation
    AddParam( ParamRange::Create( "Ellipse",      0.0f,  { 0.0f,  1.0f  } ) );
    AddParam( ParamRange::Create( "EllipseAngle", 0.0f,  { 0.0f,  1.0f  } ) );
}

ChromaticDots::~ChromaticDots() {}
