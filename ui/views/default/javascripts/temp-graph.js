var nmax = 280,
    nmin = 60,
    duration = 2000,
    now = new Date(Date.now() - duration),
    random = d3.random.normal(0, 20),
    gdata = d3.range(nmax).map(function() { return 0; }),
    bdata = d3.range(nmax).map(function() { return 0; }),
    t1data = [],
    t2data = []

var margin = {top: 10, right: 50, bottom: 40, left: 40},
    width = 940 - margin.right,
    height = 360 - margin.top - margin.bottom;

var x = d3.time.scale()
    .domain([now - (nmax - 2) * duration, now - duration])
    .range([0, width]);

var y = d3.scale.linear()
    .domain([nmin, nmax])
    .range([height, 0]);

var line = d3.svg.line()
    .interpolate("basis")
    .x(function(d, i) { return x(now - (nmax - 1 - i) * duration); })
    .y(function(d, i) { return y(d); });

var svg = d3.select("#temp-graph").append("p").append("svg")
    .attr("width", width + margin.left + margin.right)
    .attr("height", height + margin.top + margin.bottom)
    .style("margin-left", -margin.left + "px")
  .append("g")
    .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

svg.append("defs").append("clipPath")
    .attr("id", "clip")
  .append("rect")
    .attr("width", width)
    .attr("height", height);

var axis = svg.append("g")
    .attr("class", "xaxis")
    .attr("transform", "translate(0," + height + ")")
    .call(x.axis = d3.svg.axis().scale(x).orient("bottom"));

var yaxis = svg.append("g")
    .attr("class", "yaxis")
    .attr("transform", "translate(" + width + ",0)")
    .call(y.axis = d3.svg.axis().scale(y).orient("right"));

//var gpath = svg.append("g")
//    .attr("clip-path", "url(#clip)")
//  .append("path")
//    .data([gdata])
//    .attr("class", "line")

var bpath = svg.append("g")
    .attr("clip-path", "url(#clip)")
  .append("path")
    .data([bdata])
    .attr("class", "line boiler")

var gpath = svg.append("g")
    .attr("clip-path", "url(#clip)")
  .append("path")
    .data([gdata])
    .attr("class", "line grouphead")

tick();

function tick() {
  // update the domains
  now = new Date();

  x.domain([now - (nmax - 2) * duration, now - duration]);
  y.domain([nmin, nmax]);

  // push the accumulated count onto the back, and reset the count

  // Get average temp during tick duration
  var sum = 0, avg = 0, tcopy;

  tcopy = t1data;
  t1data = [];
  tcopy.forEach(function(n){
    sum += parseFloat(n); 
    avg = sum / tcopy.length;
  });
  bdata.push(avg);

  sum = 0;
  tcopy = t2data;
  t2data = [];
  tcopy.forEach(function(n){
    sum += parseFloat(n); 
    avg = sum / tcopy.length;
  });
  gdata.push(avg);


  // redraw the line
  svg.selectAll(".line")
      .attr("d", line)
      .attr("transform", null);

  // slide the line left
  bpath.transition()
      .duration(duration)
      .ease("linear")
      .attr("transform", "translate(" + x(now - (nmax - 1) * duration) + ")")
  // slide the line left
  gpath.transition()
      .duration(duration)
      .ease("linear")
      .attr("transform", "translate(" + x(now - (nmax - 1) * duration) + ")")

  // slide the x-axis left
  axis.transition()
      .duration(duration)
      .ease("linear")
      .call(x.axis)
      .each("end", tick);

  // pop the old data point off the front
  gdata.shift();
  bdata.shift();
}