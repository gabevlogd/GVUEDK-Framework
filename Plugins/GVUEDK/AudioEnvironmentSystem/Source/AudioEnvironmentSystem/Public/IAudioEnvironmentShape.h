#pragma once

#include "CoreMinimal.h"

/**
 * Pure C++ abstract interface for audio environment volume shapes.
 *
 * AAudioEnvironmentVolume implements this for Box shapes by default.
 * Extend it to support spheres, splines, convex meshes, or procedural shapes
 * without modifying the volume actor or the evaluator.
 *
 * Design note:
 *   This is deliberately NOT a UInterface. It is an internal architectural
 *   contract between the volume and its shape implementation. The evaluator
 *   only calls AAudioEnvironmentVolume::EvaluateWeightAtLocation(), which
 *   dispatches to the shape implementation internally. Downstream code is
 *   never aware of specific shape types.
 */
class AUDIOENVIRONMENTSYSTEM_API IAudioEnvironmentShape
{
public:
    virtual ~IAudioEnvironmentShape() = default;

    /**
     * Returns true if InWorldLocation is strictly inside this shape's boundary.
     * Does NOT account for the blend falloff zone.
     */
    virtual bool IsLocationInside(const FVector& InWorldLocation) const = 0;

    /**
     * Returns a normalized depth value in [0..1]:
     *   0 = at or outside the shape boundary
     *   1 = deep inside (further than BlendDistance from any face)
     *
     * This value is fed into the blend curve to produce the final weight.
     * The shape is responsible for computing the raw spatial depth; the volume
     * applies the blend mode curve on top.
     *
     * @param InBlendDistance  The configured blend falloff distance (in world units).
     */
    virtual float GetNormalizedDepth(const FVector& InWorldLocation, float InBlendDistance) const = 0;

    /**
     * Returns the closest point on the shape boundary (surface) to InWorldLocation.
     * Used for debug drawing and potential future distance-based queries.
     */
    virtual FVector GetClosestSurfacePoint(const FVector& InWorldLocation) const = 0;

    /**
     * Returns the world-space bounding sphere of this shape.
     * Used by the evaluator for fast proximity culling before the full weight evaluation.
     */
    virtual FBoxSphereBounds GetShapeBounds() const = 0;
};
