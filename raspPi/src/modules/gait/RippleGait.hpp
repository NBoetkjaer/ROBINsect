#pragma once
#include "gait.hpp"

#include <memory>

class RippleGait : public Gait
{
	using LegTrajectories = std::array <std::unique_ptr<TrajectorySegment>, NUM_LEGS>;
public:
	void UpdateGait(float elapsedTime_s) override;
	void Reset() override;
protected:
	void UpdateGroundPlane();
	void UpdateStep();
	void PrepareLegPositions();
	void UpdateLegPositions();

private:
	void CreateInitialTrajectory(LegTrajectories& initTrajectories, LegTrajectories& gaitTrajectories);
	void CreateWalkingTrajectory(LegTrajectories& trajectories);

	void InitLegSeqence();
	float PhaseOffset(int seqId);
	std::array<int, NUM_LEGS> m_legSequence;
	bool m_bInitializePending = false;

	float m_stepRetractRatio = 3.0f;
	float m_legOffsetRatio = 1.0f / NUM_LEGS;
	
    LegTrajectories m_curTrajectory;
    LegTrajectories m_nextTrajectory;
};

