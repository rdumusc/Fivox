Changelog {#changelog}
=========

# git master {#master}

* [#29](https://github.com/BlueBrain/Fivox/pull/29)
  Adapt to the renaming of zeq to ZeroEQ.
* [#28](https://github.com/BlueBrain/Fivox/pull/28)
  Adapt to servus::URI query separator change (replaced ',' by '&').

# Release 0.4 (05-04-2016)

* [#23](https://github.com/BlueBrain/Fivox/pull/23)
  Add resolution parameter in voxelize_batch script.
  Fix naming (zero-pad) when generating a series of volumes.
* [#12](https://github.com/BlueBrain/Fivox/issue/12)
  Fix progress report to include of all threads
* [#21](https://github.com/BlueBrain/Fivox/pull/21)
  New voxelize_batch.py script to generate volumes on cluster nodes.
* [#10](https://github.com/BlueBrain/Fivox/pull/10)
  voxelize: Limit volume to area covered by events.
* [#10](https://github.com/BlueBrain/Fivox/pull/10)
  Rescale to the output type range for integer volumes.
* [#9](https://github.com/BlueBrain/Fivox/pull/9)
  Add testLoader for validation purposes.
* [#4](https://github.com/BlueBrain/Fivox/pull/4)
  Add LFP functor from separate repo. Move all the voltage-specific logic to
  the FieldFunctor, so CompartmentLoader can be used with other type of reports
  (e.g. currents in LFP).
  Add radius attribute in Event, used in the CompartmentLoader.
  Adapt tests and default values to latest TestData (370ebee).
* [#7](https://github.com/BlueBrain/Fivox/pull/7):
  Add data decomposition in voxelize tool.
* [20437](https://bbpcode.epfl.ch/code/#/c/20437/)
  Expose the VSD projection attenuation coefficient (absorption + scattering)
  in the voxelize command line tool.
* [20428](https://bbpcode.epfl.ch/code/#/c/20428/)
  Fix several bugs in the VSD loader, which crashed the application when
  loading events.
  Update the reference BlueConfigVSD to point to existing data.
* [20308](https://bbpcode.epfl.ch/code/#/c/20308/)
  Use a custom type for the output volume in the voxelize tool.
* [20241](https://bbpcode.epfl.ch/code/#/c/20241/)
  Add --projection option in voxelize tool. When enabled, generate a 2D
  projection of the resulting volume, emulating the output of a VSD imaging
  process. It uses the newly added ITK projection filter
  BeerLambertProjectionImageFilter.
* [19753](https://bbpcode.epfl.ch/code/#/c/19753/)
  Fix [ISC-139]. Make sure the requested end time is inside the available time
  range when streaming spikes.
* [19713](https://bbpcode.epfl.ch/code/#/c/19713/)
  Add optional progress bar display if showProgress=1 in URI query.
* [19230](https://bbpcode.epfl.ch/code/#/c/19230/)
  Add support for volume time series generation for voxelize tool.
* [19188](https://bbpcode.epfl.ch/code/#/c/19181/)
  Bug fixes in compartment report indexing in compartment and VSD loaders.
* [19077](https://bbpcode.epfl.ch/code/#/c/19077/)
  Dt defaults to experiment/report dt, duration to 10.

# Release 0.3 (9-Nov-2015){#Release03}

* [18462](https://bbpcode.epfl.ch/code/#/c/18462/)
  Added new frequency functor, make more parameters configurable in URI
* [15814](https://bbpcode.epfl.ch/code/#/c/15814/)
  Fix underflow events by using the newly introduced MINIMUM_VOLTAGE in the
  voltage-related loaders
* [15670](https://bbpcode.epfl.ch/code/#/c/15670/)
  Tweak livre rendering parameters: blocks are now max 64 MB, and their size
  in voxels is always multiple of 8 for better alignment
* [14496](https://bbpcode.epfl.ch/code/#/c/14496/)
  New synapse loader to visualize synapse densities
* [14204](https://bbpcode.epfl.ch/code/#/c/14204/)
  Add generic voxelize tool and URIHandler
* [14204](https://bbpcode.epfl.ch/code/#/c/14204/)
  Fix bug in loaders: the event source loaded always a specific frame,
  even when a time stamp was specified.
* [14181](https://bbpcode.epfl.ch/code/#/c/14181/)
  New dye curve parameter for VSD reports in the Livre data source
* [14203](https://bbpcode.epfl.ch/code/#/c/14203/)
  Add missing method in SpikeLoader to load data for a specific timestamp
* [14181](https://bbpcode.epfl.ch/code/#/c/14181/)
  Fix computation of event sampling values for squared distances below 1
* [16290](https://bbpcode.epfl.ch/code/16290/)
  The targets can be given as a part of URL queries ( i.e.: target=Column )


# Release 0.2 (9-Jul-2015){#Release02}

* [13240](https://bbpcode.epfl.ch/code/#/c/13240/)
  [5296](https://bbpcode.epfl.ch/code/#/c/5296/)
  Add time and animation support
* [13383](https://bbpcode.epfl.ch/code/#/c/13383/)
  Fixed LOD bug, optimized tree size acording to the data bounding box
* [13762](https://bbpcode.epfl.ch/code/#/c/13762/)
  5x speedup for spike sampling
* [13877](https://bbpcode.epfl.ch/code/#/c/13877/)
  Fix voxelizeVSD tool to load the specified time
* [13669](https://bbpcode.epfl.ch/code/#/c/13669/)
  Use non-negative voltage in vsd loader
* [13102](https://bbpcode.epfl.ch/code/#/c/13102/)
  Allow different targets for spikes, make report for compartment & soma
  accessible from URI
* [10636](https://bbpcode.epfl.ch/code/#/c/10636/)
  Adding depth attenuation property to vsd
* [13947](https://bbpcode.epfl.ch/code/#/c/13947/)
  Use the circuit target when no target is provided
* [12973](https://bbpcode.epfl.ch/code/#/c/12973/)
  Change datasource schemes to lowercase
* Documentation updated

# Release 0.1 (30-Apr-2015){#Release01}

* Initial version
