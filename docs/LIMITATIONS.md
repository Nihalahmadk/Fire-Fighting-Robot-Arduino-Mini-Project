# Limitations

## Stated in the report
1. **Limited sensor range** – flame sensors detect only nearby fire.
2. **Blind spots** – areas outside the three sensors' fields of view are missed.
3. **Obstacle avoidance** – navigation in cluttered spaces is difficult; no obstacle sensor is in the block diagram.

## Identified in the audit
4. The original firmware is not in the repository, so the original behaviour cannot be reviewed. The new reference firmware is compile-checked only.
5. No evidence of physical-hardware testing; the only "result" is a simulation schematic.
6. No documented fire-out check, so the robot may keep or stop spraying incorrectly (not determinable without code).
7. IR flame sensors can be triggered by sunlight or other IR sources; no false-trigger handling is documented.
8. The gas sensor's role in decisions is undocumented.
9. Call alert is claimed in the title but not shown in the flowchart.
10. Power-supply design, current budget and protection are undocumented.
11. Water capacity of a small container limits what fire can be dealt with; the project is a demonstration-scale prototype, not a certified fire-safety device.

## Reference firmware limitations
See section 6 of [`SOFTWARE_DOCUMENTATION.md`](SOFTWARE_DOCUMENTATION.md).

## Safety note
This is an academic prototype. It must not be relied on for real fire protection.
