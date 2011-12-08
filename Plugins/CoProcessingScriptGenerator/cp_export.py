# boolean telling if we want to export rendering.
export_rendering = %1

# string->string map with key being the proxyname while value being the
# simulation input name.
simulation_input_map = { %2 };

image_file_name = '%3'

image_write_frequency = %4

# This is map of lists of write frequencies. This is used to populate the
# RequestDataDescription() method so that grids are requested only when needed.
write_frequencies = {}

# we do the views last and only if export_rendering is true
view_proxies = []

for key in simulation_input_map.values():
    write_frequencies[key] = []

def cp_locate_simulation_inputs(proxy):
    if hasattr(proxy, "cpSimulationInput"):
        return [ proxy.cpSimulationInput ]

    input_proxies = []
    for property in smtrace.servermanager.PropertyIterator(proxy):
        if property.IsA("vtkSMInputProperty"):
            ip = smtrace.servermanager.InputProperty(proxy, property)
            input_proxies = input_proxies + ip[:]

    simulation_inputs = []
    for input in input_proxies:
        cur_si = cp_locate_simulation_inputs(input.SMProxy)
        for cur in cur_si:
            if not cur in simulation_inputs:
                simulation_inputs.append(cur)
    return simulation_inputs


def cp_locate_simulation_inputs_for_view(view_proxy):
    reprProp = smtrace.servermanager.ProxyProperty(view_proxy, view_proxy.GetProperty("Representations"))
    reprs = reprProp[:]
    all_sim_inputs = []
    for repr in reprs:
        sim_inputs = cp_locate_simulation_inputs(repr)
        all_sim_inputs = all_sim_inputs + sim_inputs
    return all_sim_inputs

def cp_hook(info, ctorMethod, ctorArgs, extraCtorCommands):
    global write_frequencies, simulation_input_map, export_rendering, view_proxies
    if info.ProxyName in simulation_input_map.keys():
        # mark this proxy as a simulation input to make it easier to locate the
        # simulation input for the writers.
        info.Proxy.cpSimulationInput = simulation_input_map[info.ProxyName]
        return ('CreateProducer',\
          [ 'datadescription', '\"%s\"' % (simulation_input_map[info.ProxyName]) ], '')
    # handle writers.
    proxy = info.Proxy
    if proxy.GetXMLGroup() == 'views' and export_rendering:
        view_proxies.append(proxy)

    if not proxy.GetHints() or \
      not proxy.GetHints().FindNestedElementByName("CoProcessing"):
        return (ctorMethod, ctorArgs, extraCtorCommands)
    # this is a writer we are dealing with.
    xmlElement = proxy.GetHints().FindNestedElementByName("CoProcessing")
    xmlgroup = xmlElement.GetAttribute("group")
    xmlname = xmlElement.GetAttribute("name")

    pxm = smtrace.servermanager.ProxyManager()
    ctorMethod = None
    writer_proxy = pxm.GetPrototypeProxy(xmlgroup, xmlname)
    if writer_proxy:
        # we have a valid prototype based on the writer stub
        ctorMethod =  \
            smtrace.servermanager._make_name_valid(writer_proxy.GetXMLLabel())
    elif not writer_proxy:
        # a bit of a hack but we assume that there's a stub of some
        # writer that's not available in this build but is available
        # with the build used by the simulation code (probably through a plugin)
        # this stub must have the proper name in the coprocessing hints
        print "WARNING: Could not find", xmlname, "writer in", xmlgroup, \
            "XML group. This is not a problem as long as the writer is available with " \
            "the ParaView build used by the simulation code."
        ctorMethod =  \
            smtrace.servermanager._make_name_valid(xmlname)

    write_frequency = proxy.GetProperty("WriteFrequency").GetElement(0)
    ctorArgs = [ctorMethod, \
                "\"%s\"" % proxy.GetProperty("FileName").GetElement(0),\
                write_frequency, "cp_writers"]
    ctorMethod = "CreateWriter"

    # Locate which simulation input this write is connected to, if any. If so,
    # we update the write_frequencies datastructure accordingly.
    sim_inputs = cp_locate_simulation_inputs(proxy)
    for sim_input_name in sim_inputs:
        if not write_frequency in write_frequencies[sim_input_name]:
            write_frequencies[sim_input_name].append(write_frequency)
    return (ctorMethod, ctorArgs, '')

try:
    from paraview import smstate, smtrace
except:
    raise RuntimeError('could not import paraview.smstate')


# Start trace
smtrace.start_trace(CaptureAllProperties=True, UseGuiName=True)

# update trace globals.
smtrace.trace_globals.proxy_ctor_hook = staticmethod(cp_hook)
smtrace.trace_globals.trace_output = []

# Get list of proxy lists
proxy_lists = smstate.get_proxy_lists_ordered_by_group(WithRendering=export_rendering)
# Now register the proxies with the smtrace module
for proxy_list in proxy_lists:
    smstate.register_proxies_by_dependency(proxy_list)

# Calling append_trace causes the smtrace module to sort out all the
# registered proxies and their properties and write them as executable
# python.
smtrace.append_trace()

# Stop trace and print it to the console
smtrace.stop_trace()


for view_proxy in view_proxies:
    # Locate which simulation input this write is connected to, if any. If so,
    # we update the write_frequencies datastructure accordingly.
    sim_inputs = cp_locate_simulation_inputs_for_view(view_proxy)
    for sim_input_name in sim_inputs:
        if not image_write_frequency in write_frequencies[sim_input_name]:
            write_frequencies[sim_input_name].append(image_write_frequency)
    #write_frequencies['input'].append(image_write_frequency)

output_contents = """
try: paraview.simple
except: from paraview.simple import *

def RequestDataDescription(datadescription):
    "Callback to populate the request for current timestep"
    if datadescription.GetForceOutput() == True:
        for i in range(datadescription.GetNumberOfInputDescriptions()):
            datadescription.GetInputDescription(i).AllFieldsOn()
            datadescription.GetInputDescription(i).GenerateMeshOn()
        return

    timestep = datadescription.GetTimeStep()
%s

def DoCoProcessing(datadescription):
    "Callback to do co-processing for current timestep"
    cp_writers = []
    timestep = datadescription.GetTimeStep()

%s
    for writer in cp_writers:
        if timestep %% writer.cpFrequency == 0 or datadescription.GetForceOutput() == True:
            writer.FileName = writer.cpFileName.replace("%%t", str(timestep))
            writer.UpdatePipeline()

    if timestep %% %s == 0 or datadescription.GetForceOutput() == True:
        views = servermanager.GetRenderViews()
        views += servermanager.GetContextViews()
        imagefilename = "%s"
        for view in range(len(views)):
            fname = imagefilename.replace("%%v", str(view))
            fname = fname.replace("%%t", str(timestep))
            WriteImage(fname, views[view])

    # explicitly delete the proxies -- we do it this way to avoid problems with prototypes
    tobedeleted = GetNextProxyToDelete()
    while tobedeleted != None:
        Delete(tobedeleted)
        tobedeleted = GetNextProxyToDelete()

def GetNextProxyToDelete():
    iter = servermanager.vtkSMProxyIterator()
    iter.Begin()
    while not iter.IsAtEnd():
        if iter.GetGroup().find("prototypes") != -1:
            iter.Next()
            continue
        proxy = servermanager._getPyProxy(iter.GetProxy())
        proxygroup = iter.GetGroup()
        if proxygroup != 'timekeeper' and proxy != None and proxygroup.find("pq_helper_proxies") == -1 :
            return proxy
        iter.Next()

    return None

def CreateProducer(datadescription, gridname):
    "Creates a producer proxy for the grid"
    if not datadescription.GetInputDescriptionByName(gridname):
        raise RuntimeError, "Simulation input name '%%s' does not exist" %% gridname
    grid = datadescription.GetInputDescriptionByName(gridname).GetGrid()
    producer = PVTrivialProducer()
    producer.GetClientSideObject().SetOutput(grid)
    if grid.IsA("vtkImageData") == True or grid.IsA("vtkStructuredGrid") == True or grid.IsA("vtkRectilinearGrid") == True:
        extent = datadescription.GetInputDescriptionByName(gridname).GetWholeExtent()
        producer.WholeExtent= [ extent[0], extent[1], extent[2], extent[3], extent[4], extent[5] ]

    producer.UpdatePipeline()
    return producer


def CreateWriter(proxy_ctor, filename, freq, cp_writers):
    writer = proxy_ctor()
    writer.FileName = filename
    writer.add_attribute("cpFrequency", freq)
    writer.add_attribute("cpFileName", filename)
    cp_writers.append(writer)
    return writer
"""

timestep_expression = """
    input_name = '%s'
    if %s :
        datadescription.GetInputDescriptionByName(input_name).AllFieldsOn()
        datadescription.GetInputDescriptionByName(input_name).GenerateMeshOn()
    else:
        datadescription.GetInputDescriptionByName(input_name).AllFieldsOff()
        datadescription.GetInputDescriptionByName(input_name).GenerateMeshOff()
"""

do_coprocessing = ""
for original_line in smtrace.trace_globals.trace_output:
    for line in original_line.split("\n"):
        do_coprocessing += "    " + line + "\n";

request_data_description = ""
for sim_input in write_frequencies:
    freqs = write_frequencies[sim_input]

    if len(freqs) == 0:
        continue
    freqs.sort()
    condition_str = "(timestep % " + " == 0) or (timestep % ".join(map(str, freqs)) + " == 0)"
    request_data_description += timestep_expression % (sim_input, condition_str)


fileName = "%5"
outFile = open(fileName, 'w')
if image_write_frequency < 1:
    image_write_frequency = 1

outFile.write(output_contents % (request_data_description, do_coprocessing, image_write_frequency, image_file_name))
outFile.close()

