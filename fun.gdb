define pe
p "edge"
p *($arg0->edge)
p "is reverse"
p $arg0->reverse
end
define pv
p "is root"
p $arg0 == $arg0->parent
p "index"
p $arg0->index
p "head"
p *($arg0->head->edge)
end
