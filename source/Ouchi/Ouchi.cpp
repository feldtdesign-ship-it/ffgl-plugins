#include "Ouchi.h"
using namespace ffglqs;

static CFFGLPluginInfo PluginInfo = Source::CreatePlugin< Ouchi >( [] {
    ffglqs::PluginInfo i( "MOUC", "Ouchi" );
    i.description  = "Ouchi illusion: orthogonal rectangular dot fields cause center disk to float and slide independently of surround";
    i.about        = "Michael Weinfeld / NSGL";
    i.majorVersion = 1;
    i.minorVersion = 0;
    return i;
}() );

static const char fragmentShader[] = R"(

// ---------------------------------------------------------------------------
// Ouchi Illusion
//
// A central circular disk is filled with a high-spatial-frequency rectangular
// dot pattern oriented VERTICALLY. The surrounding field uses the same dot
// pattern oriented HORIZONTALLY (rotated 90 degrees).
//
// The visual cortex segregates the two orthogonally oriented regions into
// separate gestalten. When the observer's eyes move -- even via imperceptible
// microsaccades -- the motion vectors along the edges of the rectangular dots
// are integrated differently for the two orientations. The center disk
// appears to float, slide, and drift independently of the surround.
//
// The effect is maximized on large LED walls where involuntary eye movements
// across the screen are unavoidable.
//
// Uniforms (auto-injected): time, resolution, i_uv
// Custom: DiskRadius, TileFreq, AspectRatio, DotWidth, Invert, Drift
// ---------------------------------------------------------------------------

void main()
{
    // Center UV on (0,0), aspect-correct
    float aspect = resolution.x / resolution.y;
    vec2 p = ( i_uv - 0.5 ) * 2.0;
    p.x *= aspect;

    // --- Disk mask ---
    // Central circular region defined by DiskRadius
    float dist      = length( p );
    float edge      = 0.008;
    float disk_mask = 1.0 - smoothstep( DiskRadius - edge, DiskRadius + edge, dist );

    // --- Rectangular dot pattern ---
    // TileFreq controls spatial frequency (number of tiles across screen half)
    // DotWidth controls the duty cycle of the rectangular bars (0=thin lines, 1=filled)
    //
    // The "dot" is actually a rectangular bar -- aspect ratio controlled by AspectRatio
    // This matches the classic Ouchi stimulus more precisely than square tiles.

    // SURROUND pattern: horizontal bars (tile in Y, stripe in X)
    vec2 surround_uv = p * TileFreq;
    float sx = fract( surround_uv.x * AspectRatio );
    float sy = fract( surround_uv.y );
    float surround_bar = step( DotWidth, sx ) * step( DotWidth, sy );
    // step gives us black bars on white -- invert for white bars on black
    float surround_val = 1.0 - surround_bar;

    // CENTER pattern: vertical bars (rotate 90 degrees -- swap x/y tile axes)
    vec2 center_uv = p * TileFreq;
    float cx = fract( center_uv.x );
    float cy = fract( center_uv.y * AspectRatio );
    float center_bar = step( DotWidth, cx ) * step( DotWidth, cy );
    float center_val = 1.0 - center_bar;

    // --- Drift: subtle time-driven phase offset ---
    // A very slow sinusoidal phase nudge in the center field re-triggers
    // the floating illusion over time -- simulates the microsaccade refresh.
    // Drift=0 is static (purest illusion), Drift>0 adds autonomous float.
    float drift_offset = sin( time * Drift * 0.4 ) * 0.5;
    vec2  drift_uv     = p * TileFreq + vec2( drift_offset * 0.02, 0.0 );
    float dcx = fract( drift_uv.x );
    float dcy = fract( drift_uv.y * AspectRatio );
    float drift_bar = step( DotWidth, dcx ) * step( DotWidth, dcy );
    float drift_val = 1.0 - drift_bar;

    // Blend static center with drifted center based on Drift amount
    float drift_blend = clamp( Drift, 0.0, 1.0 );
    float final_center = mix( center_val, drift_val, drift_blend * 0.3 );

    // --- Composite center over surround ---
    float pattern = mix( surround_val, final_center, disk_mask );

    // --- Invert toggle ---
    // Flipping polarity reverses which field appears to float.
    // Also changes the perceptual direction of the slide.
    if ( Invert > 0.5 )
        pattern = 1.0 - pattern;

    fragColor = vec4( vec3( pattern ), 1.0 );
}
)";

Ouchi::Ouchi()
{
    SetFragmentShader( fragmentShader );

    // Disk
    AddParam( ParamRange::Create( "DiskRadius",  0.45f, { 0.05f, 1.0f  } ) );

    // Pattern
    AddParam( ParamRange::Create( "TileFreq",    8.0f,  { 1.0f,  40.0f } ) );
    AddParam( ParamRange::Create( "AspectRatio", 3.0f,  { 1.0f,  8.0f  } ) );  // bar elongation
    AddParam( ParamRange::Create( "DotWidth",    0.4f,  { 0.05f, 0.95f } ) );  // duty cycle

    // Behavior
    AddParam( ParamRange::Create( "Invert",      0.0f,  { 0.0f,  1.0f  } ) );  // polarity flip
    AddParam( ParamRange::Create( "Drift",       0.0f,  { 0.0f,  1.0f  } ) );  // autonomous float
}

Ouchi::~Ouchi() {}
